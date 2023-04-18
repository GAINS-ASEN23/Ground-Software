/* MAIN FRONTEND FILE
* GAINS Senior Project Capstone Ground Software GUI Frontend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main file for the Orbital Trajectory Estimation Software
*/

#include <main.h>

//----- Setup -----

// Create functions to outline what happens in window
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// temporary spot to initialize the camera position
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// global variables for timing
float deltaTime = 0.0;	// Time between current frame and last frame
float lastFrame = 0.0; // Time of last frame

// global variables for mouse control
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
double lastX = 800.0f / 2.0;
double lastY = 600.0 / 2.0;
float fov = 45.0f;

// sense if the mouse is over the gui
bool down;

// set the mode that the GUI and orbital simulation is in
// 0 = orbital simulation, 1 = free movement, 2 = cnc testing mode
int simMode = 1;

// set the reference frame that the orbital simulation is in
// 0 = Earth Centered, 1 = Moon Centered
int refFrame = 1;

// define utility functions
unsigned int loadTexture(unsigned char* image_data, int width, int height, int nrChannels, unsigned int texture);
int drawPlanet(shader shaderProgram, unsigned int VAO, unsigned int texture, float scale, float planetScale, float rotation, float positions[3], glm::mat4 model, glm::mat4 view, glm::mat4 proj, bool posFromObject, int sphereIndexCount);
void drawCommsGUI(bool& shouldSendMessage, bool& initiateIP, bool& show_reset, bool& resetComms,  int sendIP[4], int& send_port, int receiveIP[4], int& receive_port, int window_width);
int calcGUI_RefLen(ImDrawList* myDrawList, int simMode, int screen_width, int screen_height, float actualScale);

int main()
{
    // Initialize GLFW
    glfwInit();

    // Set Major OpenGL version to OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

    // Set Minor OpenGL version to OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Use the smaller subset of OpenGL commands
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Apple Compatibility - Needs to be tested
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create the GUI window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GAINS Ground Software", NULL, NULL);
    // Window Error Check
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Make the window be used by OpenGL
    glfwMakeContextCurrent(window);

    // This allows for window resizing
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load all OpenGL function pointers with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetCursorPosCallback(window, mouse_callback); // captures mouse movements while the cursor is captured
    glfwSetScrollCallback(window, scroll_callback); // captures mouse scroll wheel actions

    // Setup the GUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // openGL options
    glEnable(GL_DEPTH_TEST); // depth testing to ensure the proper order of objects

    // Generate shader program with textures from the shader class
    shader planetShaderProgram(vs_Planet_Source, fs_Planet_Source);

    // Generate shader program with color only
    shader iconShaderProgram(vs_Icon_Source, fs_Icon_Source);

    // Generate line shader program from the shader class
    shader lineShaderProgram(vs_line_source, fs_line_source);

    // Generate program for generic 3D objects from shader class - intended for use with box
    shader boxShaderProgram(vs_3d_Source, fs_3d_Source);

    // --- Create the Shaders ---
    unsigned int VBO[4], VAO[4], EBO[2];
    glGenVertexArrays(4, VAO);
    glGenBuffers(4, VBO);
    glGenBuffers(2, EBO);

    // Generate a sphere
    Sphere planet(1.0f, 36, 18); // radius, sectors, stacks, smooth(default)
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, planet.getInterleavedVertexSize(), planet.getInterleavedVertices(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, planet.getIndexSize(), planet.getIndices(), GL_STATIC_DRAW);
    int stride = planet.getInterleavedStride(); // should be 32 bytes
    glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, false, stride, (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);

    // Generate a small sphere for faraway dots
    Sphere dot(1.0f, 12, 6);
    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, dot.getInterleavedVertexSize(), dot.getInterleavedVertices(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, dot.getIndexSize(), dot.getIndices(), GL_STATIC_DRAW);
    stride = dot.getInterleavedStride(); // should be 32 bytes
    glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, false, stride, (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);

    // --- Create the Gains texture ---
    stbi_set_flip_vertically_on_load(true); // flips images loaded so that 0,0 is on the bottom left corner
    unsigned int textures[3];
    glGenTextures(3, textures); // first input is number of textures to generate, 2nd is pointer to texture / texture array
    int width, height, nrChannels;
    unsigned char* gains_image_data = stbi_load("GAINS_Small_White.png", &width, &height, &nrChannels, 0);
    textures[0] = loadTexture(gains_image_data, width, height, nrChannels, textures[0]);

    // orbital simulation mode preparations
    //if (simMode == 0) {

        // --- Create the Earth texture ---
        stbi_set_flip_vertically_on_load(false);
        unsigned char* earth_image_data = stbi_load("earth2048.bmp", &width, &height, &nrChannels, 0);
        textures[1] = loadTexture(earth_image_data, width, height, nrChannels, textures[1]);

        // --- Create the Moon texture ---
        unsigned char* moon_image_data = stbi_load("moon1024.bmp", &width, &height, &nrChannels, 0);
        textures[2] = loadTexture(moon_image_data, width, height, nrChannels, textures[2]);

    //}

    // load the satellites prism into the 3rd VAO and VBO
    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(satelliteVertices), satelliteVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Define the pointers to the transformation matrices
    glm::mat4 trans1;
    glm::mat4 trans_earth;
    glm::mat4 trans_moon;

    // Define variables for use in loop
    float rotation = 0.0f;
    float translation[] = { -1.0f, 0.0f, 0.0f };
    float earth_rotation = 0.0f;
    float moon_rotation = 0.0f;
    float ins_rotation = 180.0f;
    float moon_translation[] = { 1.0f, 0.0f, 0.0f };
    float trajectory_translation[] = { 0.8f, 0.0f, 0.0f };
    bool lock_motion = false;
    float viewScale = 1;
    float timeScale = 3600;
    float actualScale = 1;
    const int lineCount = 50;
    if (simMode == 0) {
        actualScale = 10000;
    }
    else{
        actualScale = 1000;
        timeScale = 1;
    }
    float tempVert[3 * lineCount];
    int step = 0;
    float lastTime = 0;
    int screen_width, screen_height;

    // Initialize variables for communications and multithreading
    std::string teensy_ipaddress = "127.0.0.1";//"21.0.0.103";
    int teensy_ip[4] = { 127,0,0,1 };
    int teensy_port = 8888;
    std::string receive_ipaddress = "0.0.0.0";// "21.0.0.2";
    int receive_ip[4] = { 0,0,0,0 };
    int receive_port = 8889;
    bool shouldSendMessage = false;
    bool shouldInitiateIPConnection = false;
    bool ethernet_data_ready_flag = false;
    bool showComms = true;
    bool showCommsReset = false;
    bool resetComms = false;

    // Initialize the local data struct in the stack because we just need it in the main function
    boost::array<uint8_t, 720> receive_buffer;
    int receive_size_array[10];
    size_t receive_size;
    float receive_time_float;
    GAINS_TLM_PACKET receive_tlm_packet;
    GAINS_STAR_PACKET receive_star_packet;

    // Initialize the thread safe class in the heap because we need to be able to access from anywhere (i.e. outside the main function's scope)
    ethernet_data* eth_data = new ethernet_data();

    // Initialize a vector to hold all of the data points to be plotted
    // each row holds another set of data. Column 0 holds the time, Columns [1,2,3] hold the xyz postions, Columns [4,5,6] hold the xyz velocities
    std::vector<std::vector<double>>test_data;
    std::vector<std::vector<double>>received_data; 
    int step_count = 0;
    int received_data_size = 0;
    //std::vector<std::vector<double>>predicted_data;
    // 
    //Generate Example data
    test_data.push_back({ 0, 0, 0, 0, 0.05, -0.03, 0.02 });
    for (int iterate = 1; iterate < 10; iterate++) {
        test_data.push_back({ 0, test_data.at(iterate - 1).at(1) + test_data.at(iterate - 1).at(4),
            test_data.at(iterate - 1).at(2) + test_data.at(iterate - 1).at(5),
            test_data.at(iterate - 1).at(3) + test_data.at(iterate - 1).at(6),
            0.05, -0.03, 0.02 });
    }
    save_data(test_data, "Test_data.csv");
    //received_data = load_data("Test_data.csv");

    // Create the model,view, and projection transformation matrices
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection;
    int modelLoc;
    int viewLoc;
    int projectionLoc;
    size_t packetReceivelen = 0;
    
    std::vector<std::string> date = // temporarily here until we can fix linking issues when loading "data.h"
    {
        "2022 October 01, 13:00:00 PST",
        "2022 October 02, 13:00:00 PST",
        "2022 October 03, 13:00:00 PST",
        "2022 October 04, 13:00:00 PST",
        "2022 October 05, 13:00:00 PST",
        "2022 October 06, 13:00:00 PST",
        "2022 October 07, 13:00:00 PST",
        "2022 October 08, 13:00:00 PST",
        "2022 October 09, 13:00:00 PST",
        "2022 October 10, 13:00:00 PST",
        "2022 October 11, 13:00:00 PST",
        "2022 October 12, 13:00:00 PST",
        "2022 October 13, 13:00:00 PST",
        "2022 October 14, 13:00:00 PST",
        "2022 October 15, 13:00:00 PST",
        "2022 October 16, 13:00:00 PST",
        "2022 October 17, 13:00:00 PST",
        "2022 October 18, 13:00:00 PST",
        "2022 October 19, 13:00:00 PST",
        "2022 October 20, 13:00:00 PST",
        "2022 October 21, 13:00:00 PST",
        "2022 October 22, 13:00:00 PST",
        "2022 October 23, 13:00:00 PST",
        "2022 October 24, 13:00:00 PST",
        "2022 October 24, 13:00:00 PST",
        "2022 October 26, 13:00:00 PST",
        "2022 October 27, 13:00:00 PST",
        "2022 October 28, 13:00:00 PST",
        "2022 October 29, 13:00:00 PST",
        "2022 October 30, 13:00:00 PST",
        "2022 October 31, 13:00:00 PST",
        "2022 November 01, 13:00:00 PST",
        "2022 November 02, 13:00:00 PST",
        "2022 November 03, 13:00:00 PST",
        "2022 November 04, 13:00:00 PST",
        "2022 November 05, 13:00:00 PST",
        "2022 November 06, 13:00:00 PST",
        "2022 November 07, 13:00:00 PST",
        "2022 November 08, 13:00:00 PST",
        "2022 November 09, 13:00:00 PST",
        "2022 November 10, 13:00:00 PST",
        "2022 November 11, 13:00:00 PST",
        "2022 November 12, 13:00:00 PST",
        "2022 November 13, 13:00:00 PST",
        "2022 November 14, 13:00:00 PST",
        "2022 November 15, 13:00:00 PST",
        "2022 November 16, 13:00:00 PST",
        "2022 November 17, 13:00:00 PST",
        "2022 November 18, 13:00:00 PST",
        "2022 November 19, 13:00:00 PST",
        "2022 November 20, 13:00:00 PST",
        "2022 November 21, 13:00:00 PST",
        "2022 November 22, 13:00:00 PST",
        "2022 November 23, 13:00:00 PST",
        "2022 November 24, 13:00:00 PST",
        "2022 November 24, 13:00:00 PST",
        "2022 November 26, 13:00:00 PST",
        "2022 November 27, 13:00:00 PST",
        "2022 November 28, 13:00:00 PST",
        "2022 November 29, 13:00:00 PST",
        "2022 November 30, 13:00:00 PST"
    }; 

    SPICE spiceFront;

    // initialize the spice object  
    std::vector<std::vector<double>>PosVectorMoon;
    std::vector<std::vector<double>>PosVectorEarth;
    PosVectorMoon = spiceFront.SpiceCall(date, Spice::ObjectID::MOON, Spice::FrameID::J2000, Spice::ObjectID::EARTH, Spice::AbCorrectionID::NONE);
    PosVectorEarth = spiceFront.SpiceCall(date, Spice::ObjectID::EARTH, Spice::FrameID::J2000, Spice::ObjectID::MOON, Spice::AbCorrectionID::NONE);
    float spiceTemp[3 * lineCount];
    float spiceTempData[3 * lineCount];
    float dataTimeSpace = 24.0 * 3600.0;
    for (int i = 0; i < 30; i = i + 3) {
            spiceTemp[i] = float(PosVectorEarth.at(i).at(0));
            spiceTemp[i + 1] = float(PosVectorEarth.at(i).at(1));
            spiceTemp[i + 2] = float(PosVectorEarth.at(i).at(2));
    }

    // --- Spice data testing from backend ---
    // 
    // Recording the timestamp at the start of the code
    auto beg = std::chrono::high_resolution_clock::now();

    NBODYSIM nbodyObj;

    // Integration Stuff
    double totTime = 60 * 60 * 3;
    double dt = 5;

    std::cout << "NBODYSIM Running..... \n";

    std::vector<std::vector<double>> PosVector = nbodyObj.NBODYFUNC_MSC(totTime, dt, dateEx, V_scM, R_scM);

    // Taking a timestamp after the code is ran
    auto end = std::chrono::high_resolution_clock::now();

    // Subtracting the end timestamp from the beginning, And we choose to receive the difference in microseconds
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - beg);

    // Displaying the elapsed time
    std::cout << "Elapsed Time: " << duration.count();

    // --- end spice data testing ---
    printf("\n \n Data points in orbit: %d \n \n",int(std::size(PosVector)));

    // *** start thread for ethernet stuff here. When we start the thread, input a void pointer into the new thread as an argument into the function that the thread calls
    // We can type cast this pointer to the space packet protocol type to pull it out. The function inside the thread - the data inside we want to get out should be stored
    // on the heap so it doesnt get deleted. Once we get the data we need then we can deallocate the space packet protocol data. We need a semaphore inside and outside 
    // so that the gui doesn't read the data while the data is being written in the alternate thread.
    // Do join after the while loop for c++ garbage collection
    std::thread thread_two(ethernet_backend(), eth_data);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Read inputs
        processInput(window);

        // Put into back buffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // create a new GUI frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // keep track of how long it takes to render each frame
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // adjust the users view
        view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f); // creates perspective projection - farther objects are smaller

        // --- Communication Receive ---
        teensy_ipaddress = std::to_string(teensy_ip[0]) + "." + std::to_string(teensy_ip[1]) + "." + std::to_string(teensy_ip[2]) + "." + std::to_string(teensy_ip[3]);
        receive_ipaddress = std::to_string(receive_ip[0]) + "." + std::to_string(receive_ip[1]) + "." + std::to_string(receive_ip[2]) + "." + std::to_string(receive_ip[3]);

        // Communications Update Ip Address and Port
        if (shouldInitiateIPConnection) {
            // add in code to update ipaddress and port of thread
            eth_data->set_ip(receive_ipaddress, receive_port);
            shouldInitiateIPConnection = false;
        }

        // Check for the state of the flag to see whether we have new data available
        eth_data->get_ready_flag(ethernet_data_ready_flag);
        if (ethernet_data_ready_flag == true)
        {
            // Get the data
            /*eth_data->get_data(receive_buffer, receive_size_array);
            std::cout << "Receive Data Size = {";
            for (int i = 0; i < 9; i++) {
                std::cout << receive_size_array[i] << ", ";
            }
            std::cout << receive_size_array[9] << "} \n";*/

            eth_data->get_data(receive_buffer, receive_size);
            std::cout << "Received Data Size = " << receive_size << std::endl;

            if (receive_size == 4) {
                receive_time_float = 0;
                memcpy(&receive_time_float, &receive_buffer[0], sizeof(float));
                std::cout << "Received Time: '" << receive_time_float << "\n";
            }
            else {
                if (receive_buffer[12] == 0) {
                    // receive telemetry packet
                    receive_tlm_packet = read_TLM_Packet(receive_buffer);
                    std::cout << "Successfully read in the data packet. These are the contents of the TLM Packet: \n";
                    print_GAINS_TLM_PACKET(receive_tlm_packet);
                    headerData recvHdr = readHeader(receive_tlm_packet.FullHeader.SpacePacket.Hdr);
                    received_data.push_back({receive_tlm_packet.FullHeader.Sec.Time,
                        receive_tlm_packet.position_x,receive_tlm_packet.position_y ,receive_tlm_packet.position_z,
                        receive_tlm_packet.velocity_x, receive_tlm_packet.velocity_y, receive_tlm_packet.velocity_z });
                    received_data_size = received_data.size();
                    std::cout << received_data.at(received_data_size - 1).at(0) << "," << received_data.at(received_data_size - 1).at(1) << "," << received_data.at(received_data_size - 1).at(2) << ","
                        << received_data.at(received_data_size - 1).at(3) << "," << received_data.at(received_data_size - 1).at(4) << "," << received_data.at(received_data_size - 1).at(5) << "," << received_data.at(received_data_size - 1).at(6) << '\n';
                }
                else if (receive_buffer[12] == 1) {
                    // receive star tracker packet
                    receive_star_packet = read_STAR_Packet(receive_buffer);
                    std::cout << "Successfully read in the data packet. These are the contents of the STAR packet: \n";
                    print_GAINS_STAR_PACKET(receive_star_packet);
                }
            }

            // Print the new data (to test)
            //std::cout << "Main loop time: " << currentFrame << " Flag: " << ethernet_data_ready_flag << " X: " << ethernet_packet.position_x << " Y: " << ethernet_packet.position_y << " Z: " << ethernet_packet.position_z << std::endl;

            // Set the flag back to not ready because we have read the data and are waiting for new data
            eth_data->set_ready_flag(false);
        }

        // --- Communication Send ---
        if (shouldSendMessage) {
            //float input_float = currentFrame;
            //std::cout << "Input float is '" << input_float << "'\nSending it to Sender Function...\n";
            //Send_Float(input_float, teensy_ipaddress, teensy_port);
            //printf("Sent float at time: %f \n", currentFrame);

            //GAINS_TLM_PACKET tlm_packet = GAINS_TLM_PACKET_constructor(1.1, 2.2, 3.3, 4.4, 5.5, 6.6, currentFrame, 0, 1, 0, 0, 0, 0);
            //headerData sendHdr = readHeader(tlm_packet.FullHeader.SpacePacket.Hdr);
            ////print_GAINS_TLM_PACKET(tlm_packet);
            //Send_TLM_Packet(tlm_packet, teensy_ipaddress, teensy_port);
            //printf("Sent tlm data packet at time: %f \n", currentFrame);

            //GAINS_STAR_PACKET star_packet = GAINS_STAR_PACKET_constructor(1.1, 2.2, 3.3, 4.4, currentFrame, 0, 1, 0, 0, 0, 0);
            ////print_GAINS_STAR_PACKET(star_packet);
            //Send_STAR_Packet(star_packet, teensy_ipaddress, teensy_port);
            //printf("Sent star tracker packet at time: %f \n", currentFrame);

            //for (int i = 0; i < test_data.size(); i++) {
            if (step_count < test_data.size()) {
                GAINS_TLM_PACKET tlm_packet = GAINS_TLM_PACKET_constructor(test_data.at(step_count).at(1), test_data.at(step_count).at(2), test_data.at(step_count).at(3),
                    test_data.at(step_count).at(4), test_data.at(step_count).at(5), test_data.at(step_count).at(6), currentFrame, 0, 1, 0, 0, 0, 0);
                Send_TLM_Packet(tlm_packet, teensy_ipaddress, teensy_port);
                printf("Sent tlm data packet at time: %f \n", currentFrame);
                step_count++;
            }
            //}

            shouldSendMessage = false;
        }

        if (resetComms) {
            /*eth_data->set_close_thread();
            thread_two.join();
            const auto start = std::chrono::high_resolution_clock::now();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            const auto end = std::chrono::high_resolution_clock::now();
            std::thread thread_two(ethernet_backend(), eth_data);*/
        }

        // draw objects for orbital simulation mode
        if (simMode == 0) {

            // Calculate the current step of motion given the time and speed of animation
            if (lock_motion == false) {
                float timeRemainder = currentFrame - lastTime;
                earth_rotation = fmod(earth_rotation + timeScale * (360.0f / (dataTimeSpace)) * deltaTime,360.0f);
                moon_rotation = fmod(moon_rotation + timeScale * (360.0f / (27 * dataTimeSpace)) * deltaTime, 360.0f);
                if (timeRemainder >= (dt / timeScale)) {
                    lastTime = currentFrame;
                    step = step + std::floor(timeRemainder * timeScale);
                }
            }

            // Calculate a vector of positional vectors from the moon to the earth
            int currentStep2 = step % (std::size(PosVectorEarth));
            size_t moon_temp_step;
            for (int i = 0; i < lineCount; i++) {
                moon_temp_step = i + currentStep2;
                if (moon_temp_step < (std::size(PosVectorEarth))) {
                    spiceTemp[i * 3] = float(PosVectorEarth.at(moon_temp_step).at(0));
                    spiceTemp[i * 3 + 1] = float(PosVectorEarth.at(moon_temp_step).at(1));
                    spiceTemp[i * 3 + 2] = float(PosVectorEarth.at(moon_temp_step).at(2));
                }
                else {
                    while (moon_temp_step >= (std::size(PosVectorEarth))) {
                        moon_temp_step = moon_temp_step - std::size(PosVectorEarth);
                    }
                    spiceTemp[i * 3] = float(PosVectorEarth.at(moon_temp_step).at(0));
                    spiceTemp[i * 3 + 1] = float(PosVectorEarth.at(moon_temp_step).at(1));
                    spiceTemp[i * 3 + 2] = float(PosVectorEarth.at(moon_temp_step).at(2));
                }
            }

            // Calculate a vector of positional vectors to the Object - what is this object? No one knows O_O
            int currentStep = step % (std::size(PosVector));
            size_t obj_temp_step;
            for (int i = 0; i < lineCount; i = i + 1) {
                obj_temp_step = (i*(10) + currentStep);
                if (obj_temp_step < (std::size(PosVector))) {
                    spiceTempData[i * 3] =     float(PosVector.at(obj_temp_step).at(0));
                    spiceTempData[i * 3 + 1] = float(PosVector.at(obj_temp_step).at(1));
                    spiceTempData[i * 3 + 2] = float(PosVector.at(obj_temp_step).at(2));
                }
                else {
                    while (obj_temp_step >= (std::size(PosVector))) {
                        obj_temp_step = obj_temp_step - std::size(PosVector);
                    }
                    spiceTempData[i * 3] =     float(PosVector.at(obj_temp_step).at(0));
                    spiceTempData[i * 3 + 1] = float(PosVector.at(obj_temp_step).at(1));
                    spiceTempData[i * 3 + 2] = float(PosVector.at(obj_temp_step).at(2));
                }
            }

            // Draw the "object"
            iconShaderProgram.use();
            modelLoc = glGetUniformLocation(iconShaderProgram.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            viewLoc = glGetUniformLocation(iconShaderProgram.ID, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            projectionLoc = glGetUniformLocation(iconShaderProgram.ID, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            iconShaderProgram.setVec3("color", glm::vec3(0.2, 1.0, 0.2));
            trans_earth = glm::mat4(1.0f);
            trans_earth = glm::translate(trans_earth, (1 / actualScale) * glm::vec3(spiceTempData[0], spiceTempData[1], spiceTempData[2]));
            trans_earth = glm::scale(trans_earth, 0.025f * glm::vec3(1, 1, 1));
            iconShaderProgram.setMat4("transform", trans_earth);
            glBindVertexArray(VAO[2]);
            glDrawElements(GL_TRIANGLES, dot.getIndexCount(), GL_UNSIGNED_INT, (void*)0);

            //draw the object trajectory
            glBindVertexArray(VAO[0]);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(spiceTempData), spiceTempData, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // position attribute
            glEnableVertexAttribArray(0);
            lineShaderProgram.use();
            modelLoc = glGetUniformLocation(lineShaderProgram.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            viewLoc = glGetUniformLocation(lineShaderProgram.ID, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            projectionLoc = glGetUniformLocation(lineShaderProgram.ID, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            lineShaderProgram.setVec3("color", glm::vec3(0.8, 0.0, 0.0));
            trans_earth = glm::mat4(1.0f);
            if (refFrame == 0) {
                trans_earth = glm::scale(trans_earth, (1 / actualScale) * glm::vec3(1, 1, 1));
            }
            else {
                trans_earth = glm::scale(trans_earth, (1 / actualScale) * glm::vec3(1, 1, 1));
            }
            lineShaderProgram.setMat4("transform", trans_earth);
            glDrawArrays(GL_LINE_STRIP, 0, lineCount);


            float earthScale = viewScale * (1 / actualScale) * 6371;
            if (earthScale > 0.025) {
                // --- Draw the Earth ---
                planetShaderProgram.use();
                modelLoc = glGetUniformLocation(planetShaderProgram.ID, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                viewLoc = glGetUniformLocation(planetShaderProgram.ID, "view");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
                projectionLoc = glGetUniformLocation(planetShaderProgram.ID, "projection");
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
                trans_earth = glm::mat4(1.0f);
                if (refFrame == 0) {
                    trans_earth = glm::translate(trans_earth, (1 / actualScale) * glm::vec3(0.0f, 0.0f, 0.0f));
                } else {
                    trans_earth = glm::translate(trans_earth, (1 / actualScale) * glm::vec3(spiceTemp[27], spiceTemp[28], spiceTemp[29]));
                }
                trans_earth = glm::translate(trans_earth, (1 / actualScale) * glm::vec3(0.0f, 0.0f, 0.0f));
                trans_earth = glm::rotate(trans_earth, glm::radians(earth_rotation), glm::vec3(0.0, 0.0, 1.0));
                trans_earth = glm::scale(trans_earth, earthScale * glm::vec3(1, 1, 1));
                planetShaderProgram.setMat4("transform", trans_earth);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textures[1]);
                glBindVertexArray(VAO[1]);
                //glDrawElements(GL_TRIANGLES, planet.getIndexCount(), GL_UNSIGNED_INT, (void*)0);
            }
            else {
                // --- Draw an icon for the Earth ---
                iconShaderProgram.use();
                modelLoc = glGetUniformLocation(iconShaderProgram.ID, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                viewLoc = glGetUniformLocation(iconShaderProgram.ID, "view");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
                projectionLoc = glGetUniformLocation(iconShaderProgram.ID, "projection");
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
                iconShaderProgram.setVec3("color", glm::vec3(0.2, 0.2, 0.8));
                trans_earth = glm::mat4(1.0f);
                if (refFrame == 0) {
                    trans_earth = glm::translate(trans_earth, (1 / actualScale) * glm::vec3(0.0f, 0.0f, 0.0f));
                } else {
                    trans_earth = glm::translate(trans_earth, (1 / actualScale) * glm::vec3(spiceTemp[27], spiceTemp[28], spiceTemp[29]));
                }
                trans_earth = glm::scale(trans_earth, 0.025f * glm::vec3(1, 1, 1));
                iconShaderProgram.setMat4("transform", trans_earth);
                glBindVertexArray(VAO[2]);
                //glDrawElements(GL_TRIANGLES, dot.getIndexCount(), GL_UNSIGNED_INT, (void*)0);
            }


            // --- Draw Spice Data ---
            if (refFrame != 0) {
                for (int i = 0;i < lineCount; i++) {
                    spiceTemp[3 * i] = spiceTemp[27] - spiceTemp[3 * i];
                    spiceTemp[3 * i + 1] = spiceTemp[28] - spiceTemp[3 * i + 1];
                    spiceTemp[3 * i + 2] = spiceTemp[29] - spiceTemp[3 * i + 2];
                }
            }

            glBindVertexArray(VAO[0]);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(spiceTemp), spiceTemp, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // position attribute
            glEnableVertexAttribArray(0);
            lineShaderProgram.use();
            modelLoc = glGetUniformLocation(lineShaderProgram.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            viewLoc = glGetUniformLocation(lineShaderProgram.ID, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            projectionLoc = glGetUniformLocation(lineShaderProgram.ID, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            lineShaderProgram.setVec3("color", glm::vec3(0.8, 0.0, 0.0));
            trans_moon = glm::mat4(1.0f);
            //trans3 = glm::translate(trans3, (1 / actualScale) * glm::vec3(1.0, 0.0, 0.0));
            if (refFrame == 0) {
                trans_moon = glm::scale(trans_moon, (-1 / actualScale) * glm::vec3(1, 1, 1));
            } else {
                trans_moon = glm::scale(trans_moon, (1 / actualScale) * glm::vec3(1, 1, 1));
            }
            lineShaderProgram.setMat4("transform", trans_moon);
            //glDrawArrays(GL_LINE_STRIP, 0, lineCount);

            float moonScale = viewScale * (1 / actualScale) * 1737.4f;

            if (moonScale > 0.025) {
                // --- Draw the Moon ---
                /*
                planetShaderProgram.use();
                modelLoc = glGetUniformLocation(planetShaderProgram.ID, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                viewLoc = glGetUniformLocation(planetShaderProgram.ID, "view");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
                projectionLoc = glGetUniformLocation(planetShaderProgram.ID, "projection");
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
                trans_moon = glm::mat4(1.0f);
                if (refFrame == 0) {
                    trans_moon = glm::translate(trans_moon, (1 / actualScale) * glm::vec3(-spiceTemp[27], -spiceTemp[28], -spiceTemp[29]));
                } else {
                    trans_moon = glm::translate(trans_moon, (1 / actualScale) * glm::vec3(0, 0, 0));
                }
                trans_moon = glm::rotate(trans_moon, glm::radians(moon_rotation), glm::vec3(0.0, 0.0, 1.0));
                trans_moon = glm::scale(trans_moon, moonScale * glm::vec3(1, 1, 1));
                planetShaderProgram.setMat4("transform", trans_moon);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textures[2]);
                glBindVertexArray(VAO[1]);
                glDrawElements(GL_TRIANGLES, planet.getIndexCount(), GL_UNSIGNED_INT, (void*)0);
                */

                float inputVec3[3] = { spiceTemp[27], spiceTemp[28], spiceTemp[29] };
                //if (refFrame == 0) {
                    drawPlanet(planetShaderProgram, VAO[1], textures[2], actualScale, moonScale, moon_rotation, inputVec3, model, view, projection, true, planet.getIndexCount());
                //}
                //else {
                    //drawPlanet(planetShaderProgram, VAO[1], textures[2], actualScale, moonScale, moon_rotation, inputVec3, model, view, projection, true, planet.getIndexCount());
                //}
            }
            else {
                // --- Draw an icon for the Moon ---
                iconShaderProgram.use();
                modelLoc = glGetUniformLocation(iconShaderProgram.ID, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                viewLoc = glGetUniformLocation(iconShaderProgram.ID, "view");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
                projectionLoc = glGetUniformLocation(iconShaderProgram.ID, "projection");
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
                iconShaderProgram.setVec3("color", glm::vec3(0.6, 0.6, 0.6));
                trans_moon = glm::mat4(1.0f);
                if (refFrame == 0) {
                    trans_moon = glm::translate(trans_moon, (1 / actualScale) * glm::vec3(-spiceTemp[27], -spiceTemp[28], -spiceTemp[29]));
                } else {
                    trans_moon = glm::translate(trans_moon, (1 / actualScale) * glm::vec3(0, 0, 0));
                }
                trans_moon = glm::scale(trans_moon, 0.025f * glm::vec3(1, 1, 1));
                iconShaderProgram.setMat4("transform", trans_moon);
                glBindVertexArray(VAO[2]);
                glDrawElements(GL_TRIANGLES, dot.getIndexCount(), GL_UNSIGNED_INT, (void*)0);
            }
        }
        else if (simMode == 1 || simMode == 2) {

            if (lock_motion == false) {
                float speedUp = 10;
                float timeRemainder = currentFrame - lastTime;
                if (timeRemainder >= (1 / (timeScale * speedUp))) {
                    lastTime = currentFrame;
                    step = step + std::floor(timeRemainder * timeScale * speedUp);
                }
            }

            // Calculate the current points to show on the 2D line
            if (simMode == 2) {
                ins_rotation = fmod(step * (360.0f / 64.0f), 360.0f) + 270.0f;
                int currentStep = step % (std::size(circleVert) / 3);
                int line_temp_step;
                for (int i = 0; i < 3 * lineCount; i++) {
                    line_temp_step = i + currentStep * 3;
                    if (line_temp_step < (std::size(circleVert))) {
                        tempVert[i] = circleVert[line_temp_step];
                    }
                    else {
                        line_temp_step = line_temp_step - std::size(circleVert);
                        tempVert[i] = circleVert[line_temp_step];
                    }
                }
            } else {
                std::cout << "The size of received_data is: " << received_data_size << std::endl;
                for (int i = 0; i < lineCount; i++) {
                    if ((i < (received_data_size)) && (received_data_size > 0)) {
                        tempVert[3*i] = received_data.at((received_data_size-1)-i).at(1);
                        tempVert[3*i + 1] = received_data.at((received_data_size - 1) - i).at(2);
                        tempVert[3*i + 2] = received_data.at((received_data_size - 1) - i).at(3);
                    }
                    else {
                        tempVert[3*i] = 0;
                        tempVert[3*i+1] = 0;
                        tempVert[3*i+2] = 0;
                    }
                }
            }

            // --- Draw the 2D Line ---
            glBindVertexArray(VAO[0]);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(tempVert), tempVert, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // position attribute
            glEnableVertexAttribArray(0);
            lineShaderProgram.use();
            modelLoc = glGetUniformLocation(lineShaderProgram.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            viewLoc = glGetUniformLocation(lineShaderProgram.ID, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            projectionLoc = glGetUniformLocation(lineShaderProgram.ID, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            lineShaderProgram.setVec3("color", glm::vec3(0.8, 0.1, 0.1));
            trans1 = glm::mat4(1.0f);
            if (simMode == 2) {
                trans1 = glm::scale(trans1, 250 * (1 / actualScale) * glm::vec3(1, 1, 1));
            }
            else {
                trans1 = glm::scale(trans1, 1000 * (1 / actualScale) * glm::vec3(1, 1, 1));
            }
            lineShaderProgram.setMat4("transform", trans1);
            glDrawArrays(GL_LINE_STRIP, 0, lineCount);

            boxShaderProgram.use();
            modelLoc = glGetUniformLocation(boxShaderProgram.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            viewLoc = glGetUniformLocation(boxShaderProgram.ID, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            projectionLoc = glGetUniformLocation(boxShaderProgram.ID, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            boxShaderProgram.setVec3("color", glm::vec3(0.1, 0.1, 0.8));
            trans1 = glm::mat4(1.0f);
            if (simMode == 2) {
                trans1 = glm::rotate(trans1, glm::radians(ins_rotation), glm::vec3(0.0, 0.0, 1.0));
                trans1 = glm::translate(trans1, 300 * (1 / actualScale) * glm::vec3(tempVert[0], tempVert[1], tempVert[2]));
            }
            else {
                trans1 = glm::translate(trans1, 1000 * (1 / actualScale) * glm::vec3(tempVert[0], tempVert[1], tempVert[2]));
            }
            trans1 = glm::scale(trans1, 100 * (1 / actualScale) * glm::vec3(1, 1, 1));
            boxShaderProgram.setMat4("transform", trans1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[0]);
            glBindVertexArray(VAO[3]);
            glDrawArrays(GL_TRIANGLES, 0, 36); 

        }

        // Invisible GUI overlay that displays the reference scale on the bottom right of the screen
        glfwGetFramebufferSize(window, &screen_width, &screen_height);
        ImGui::SetNextWindowPos({ 0,0 }, ImGuiCond_Once);
        ImGui::SetNextWindowSize({ float(screen_width),float(screen_height) }, 0);
        ImGui::Begin("Drawlist Window", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground); // creates the GUI and names it
        ImDrawList* myDrawList = ImGui::GetWindowDrawList();
        int ref_length = calcGUI_RefLen(myDrawList, simMode, screen_width, screen_height, actualScale);
        ImVec2 a = ImVec2((screen_width - ref_length) - 100, screen_height - 50);
        ImVec2 b = ImVec2(screen_width - 100, screen_height - 50);
        if (simMode == 0) {
            a = ImVec2((screen_width - ref_length) - 50, screen_height - 50);
            b = ImVec2(screen_width - 50, screen_height - 50);
        }
        myDrawList->AddLine(a, b, ImColor(1.0f, 1.0f, 1.0f, 1.0f), 1.0f); // Distance Scale Center Line
        myDrawList->AddLine(ImVec2(a[0], a[1] - 10), ImVec2(a[0], a[1] + 10), ImColor(1.0f, 1.0f, 1.0f, 1.0f), 1.0f); // Distance Scale Left Verticle Line
        myDrawList->AddLine(ImVec2(b[0], b[1] - 10), ImVec2(b[0], b[1] + 10), ImColor(1.0f, 1.0f, 1.0f, 1.0f), 1.0f); // Distance Scale Right Verticle Line
        ImGui::End();

        if (simMode == 0) {
            // render the GUI 
            ImGui::SetNextWindowPos({ 0,0 }, ImGuiCond_Once); // ImGui window origin starting from top left of screen
            ImGui::SetNextWindowSize({ 570,240 }, ImGuiCond_Once); 
            ImGui::Begin("Orbital Simulation GUI");

            if (ImGui::Button("Sim Mode")) {
                simMode = 0;
                actualScale = 10000;
                timeScale = 3600;
            }
            ImGui::SameLine();
            if (ImGui::Button("Free Roam Mode")) {
                simMode = 1;
                actualScale = 1000;
                timeScale = 1;
            }
            ImGui::SameLine();
            if (ImGui::Button("Test Mode")) {
                simMode = 2;
                actualScale = 1000;
                timeScale = 1;
            }
            ImGui::Checkbox("Lock Planet Movement", &lock_motion);
            ImGui::SameLine();
            ImGui::Checkbox("Show Comms", &showComms);
            ImGui::SliderFloat("Scale (km)", &actualScale, 10e2, 10e9, "%1.0f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("Frames Per Second", &timeScale, 0.1, 10e4, "%1.0f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("Moon Rotation", &moon_rotation, 0, 360);
            ImGui::Text("Choose Reference Frame Center: ");
            ImGui::SameLine();
            if (ImGui::Button(" Earth ")) {
                refFrame = 0;
            }
            ImGui::SameLine();
            if (ImGui::Button(" Moon ")) {
                refFrame = 1;
            }

        } else if (simMode == 1 || simMode == 2) {
            // render the GUI
            ImGui::SetNextWindowPos({ 0,0 }, ImGuiCond_Once); // ImGui window origin starting from top left of screen
            ImGui::SetNextWindowSize({ 570,160 }, ImGuiCond_Once);

            if (simMode == 1) {
                ImGui::Begin("Free Roam GUI");
            }
            else {
                ImGui::Begin("Test Mode GUI");
            }
            if (ImGui::Button("Sim Mode")) {
                simMode = 0;
                actualScale = 10000;
                timeScale = 3600;
            }
            ImGui::SameLine();
            if (ImGui::Button("Free Roam Mode")) {
                simMode = 1;
                actualScale = 1000;
                timeScale = 1;
            }
            ImGui::SameLine();
            if (ImGui::Button("Test Mode")) {
                simMode = 2;
                actualScale = 1000;
                timeScale = 1;
            }
            if (simMode == 2) {
                ImGui::Button("Save Data");
                ImGui::SameLine();
                ImGui::Button("Load Data");
            }
            ImGui::Checkbox("Lock Movement", &lock_motion);
            ImGui::SameLine();
            ImGui::Checkbox("Show Comms", &showComms);
            ImGui::SliderFloat("Scale (mm)", &actualScale, 1, 10e5, "%1.0f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("Time Speed", &timeScale, 0.1, 10);

        }
        //std::cout << "Window Width = " << ImGui::GetWindowWidth() << ", Window Height = " << ImGui::GetWindowHeight() << std::endl; // fixme
        if (showComms) {
            int window_width = ImGui::GetWindowWidth();
            drawCommsGUI(shouldSendMessage, shouldInitiateIPConnection, showCommsReset, resetComms, teensy_ip, teensy_port, receive_ip, receive_port, window_width);
        }
        ImGui::End();

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll for IO events (keys pressed/released, mouse moved etc.) - Move from back buffer to front buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    printf("Client Began Joining \n");
    eth_data->set_close_thread();
    thread_two.join();
    printf("Client Finished Joining \n");

    // stop rendering ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // deallocate the VAOs and VBOs
    glDeleteVertexArrays(4, VAO);
    glDeleteBuffers(4, VBO);
    glDeleteBuffers(2, EBO);

    // delete the shader program just in case
    planetShaderProgram.~shader();
    lineShaderProgram.~shader();

    // Clear all previously allocated GLFW resources
    glfwTerminate();
    return 0;
}

// Query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
//Purpose: When keyboard keys are pressed, the appropriate action is taken here

    // leaves the window and stops the program if the user hits escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // take in keyboard presses to move the camera
    //const float cameraSpeed = 0.05f; // adjust accordingly
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
}

// Whenever the window size changed this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
//Purpose: Adjusts the window to the new dimensions when a user changes dimensions on their end
 
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
// Purpose: Activates when the left mouse button is clicked and allows the view to be moved by dragging on the screen

    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse) {

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_RELEASE) // this prevents the camera from moving except when we hold the left mouse button
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // captures the cursor in an fps mode type way
            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            double xoffset = xpos - lastX;
            double yoffset = lastY - ypos;
            lastX = xpos;
            lastY = ypos;

            double sensitivity = 0.1f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            // adjust the global yaw and pitch values
            yaw += xoffset;
            pitch += yoffset;

            // limit the vertical movement so we don't have awkward movements
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            // adjust the screen position from mouse movements. Note that we include yaw and pitch but we ignore roll for now
            glm::vec3 direction;
            direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            direction.y = sin(glm::radians(pitch));
            direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraFront = glm::normalize(direction);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // releases the cursor
            firstMouse = true;
        }
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
// Purpose: This function takes the mouse scroll wheel input and adjusts the field of view accordingly
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f) // 45.0f is the normal limit
        fov = 45.0f;
}

unsigned int loadTexture(unsigned char* image_data, int width, int height, int nrChannels, unsigned int texture)
{
// Purpose: Loads in texture files and puts them into a temporary unsigned int for use in the program

    // create the texture
    glBindTexture(GL_TEXTURE_2D, texture);

    // set texture options for wrapping and filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // can be LINEAR or NEAREST

    // load and generate the texture
    if (image_data)
    {
        if (nrChannels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data); // Inputs: 1st specifies texture target,
            // 2nd is mipmap level, 3rd is openGL image format (default = GL_RGB), 4th is width, 5th is height, 6th must be 0, 7th is format,
            // 8th is data type, 9th is the actual image data
            // use GL_RGBA if file uses an alpha (transparency) channel like .pngs
        }else{
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
        }
        glGenerateMipmap(GL_TEXTURE_2D); // Mipmaps are very important if we are putting textures on large objects far away
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(image_data); // Frees the image memory now that a texture has been made

    return texture;
}

int drawPlanet(shader shaderProgram, unsigned int VAO, unsigned int texture, float scale, float planetScale, float rotation, float positions[3], glm::mat4 model, glm::mat4 view, glm::mat4 proj, bool posFromObject, int sphereIndexCount) {
    // This is a working function to draw the rotating moon or earth. Since this function requires soo many inputs, it may be better if we can make a class instead
    
    // --- Draw the Planet ---
    shaderProgram.use();
    int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    int projLoc = glGetUniformLocation(shaderProgram.ID, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
    glm::mat4 trans_mat = glm::mat4(1.0f);
    if (posFromObject) {
        trans_mat = glm::translate(trans_mat, (1 / scale) * glm::vec3(-positions[0], -positions[1], -positions[2]));
    }
    else {
        trans_mat = glm::translate(trans_mat, (1 / scale) * glm::vec3(positions[0], positions[1], positions[2]));
    }
    trans_mat = glm::rotate(trans_mat, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
    trans_mat = glm::scale(trans_mat, planetScale * glm::vec3(1, 1, 1));
    shaderProgram.setMat4("transform", trans_mat);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, (void*)0);

    //return 1; // a return value seems to prevent plotting - but why?
}

void drawCommsGUI(bool& shouldSendMessage, bool& initiateIP, bool& show_reset, bool& resetComms, int sendIP[4], int& send_port, int receiveIP[4], int& receive_port, int window_width) {
    // This function draws the comms portion of the GUI onto the current GUI. This enables user input for binding IPs and for sending and receiving
    if (ImGui::Button(" Initiate Receiver ")) {
        initiateIP = true;
    }
    ImGui::SameLine();
    if (ImGui::Button(" Send Test Message ")) {
        shouldSendMessage = true;
    }
    ImGui::SameLine();
    ImGui::Checkbox(" Show Comms Reset ", &show_reset);
    if (show_reset) {
        if (ImGui::Button(" Reset Comms ")) {
            resetComms = true;
        }
    }
    int item_width = std::floor((window_width-150) / 4);
    if (item_width < 1) {
        item_width = 1;
    }

    ImGui::Text("Teensy IpAddress");
    ImGui::PushItemWidth(item_width);
    ImGui::InputInt("S 1", &sendIP[0]);
    ImGui::SameLine();
    ImGui::PushItemWidth(item_width);
    ImGui::InputInt("S 2", &sendIP[1]);
    ImGui::SameLine();
    ImGui::PushItemWidth(item_width);
    ImGui::InputInt("S 3", &sendIP[2]);
    ImGui::SameLine();
    ImGui::PushItemWidth(item_width);
    ImGui::InputInt("S 4", &sendIP[3]);
    ImGui::InputInt("Teensy Port", &send_port);

    ImGui::Text("Receiving IpAddress");
    ImGui::PushItemWidth(item_width);
    ImGui::InputInt("R 1", &receiveIP[0]);
    ImGui::SameLine();
    ImGui::PushItemWidth(item_width);
    ImGui::InputInt("R 2", &receiveIP[1]);
    ImGui::SameLine();
    ImGui::PushItemWidth(item_width);
    ImGui::InputInt("R 3", &receiveIP[2]);
    ImGui::SameLine();
    ImGui::PushItemWidth(item_width);
    ImGui::InputInt("R 4", &receiveIP[3]);
    ImGui::InputInt("Receiving Port", &receive_port);
}

int calcGUI_RefLen(ImDrawList* myDrawList, int simMode, int screen_width, int screen_height, float actualScale) {
    // This function calculates the reference length that is shown in the bottom right of the GUI
    // It also draws the text for this reference line at the middle point of the reference line
    int ref_length = 1e6;
    if (simMode == 0) {
        if ((actualScale < 1e4)) {
            ref_length = (screen_width / 2) * (1e3 / actualScale);
            myDrawList->AddText(ImVec2((screen_width - 75) - (ref_length / 2), screen_height - 80), ImColor(1.0f, 1.0f, 1.0f, 1.0f), "1e3 km");
        }
        else if ((actualScale < 1e5)) {
            ref_length = (screen_width / 2) * (1e4 / actualScale);
            myDrawList->AddText(ImVec2((screen_width - 70) - (ref_length / 2), screen_height - 80), ImColor(1.0f, 1.0f, 1.0f, 1.0f), "1e4 km");
        }
        else if ((actualScale < 1e6)) {
            ref_length = (screen_width / 2) * (1e5 / actualScale);
            myDrawList->AddText(ImVec2((screen_width - 70) - (ref_length / 2), screen_height - 80), ImColor(1.0f, 1.0f, 1.0f, 1.0f), "1e5 km");
        }
        else if ((actualScale < 1e7)) {
            ref_length = (screen_width / 2) * (1e6 / actualScale);
            myDrawList->AddText(ImVec2((screen_width - 70) - (ref_length / 2), screen_height - 80), ImColor(1.0f, 1.0f, 1.0f, 1.0f), "1e6 km");
        }
        else if ((actualScale < 1e8)) {
            ref_length = (screen_width / 2) * (1e7 / actualScale);
            myDrawList->AddText(ImVec2((screen_width - 70) - (ref_length / 2), screen_height - 80), ImColor(1.0f, 1.0f, 1.0f, 1.0f), "1e7 km");
        }
        else if ((actualScale < 1e9)) {
            ref_length = (screen_width / 2) * (1e8 / actualScale);
            myDrawList->AddText(ImVec2((screen_width - 70) - (ref_length / 2), screen_height - 80), ImColor(1.0f, 1.0f, 1.0f, 1.0f), "1e8 km");
        }
        else {
            ref_length = (screen_width / 2) * (1e9 / actualScale);
            myDrawList->AddText(ImVec2((screen_width - 70) - (ref_length / 2), screen_height - 80), ImColor(1.0f, 1.0f, 1.0f, 1.0f), "1e9 km");
        }
    }
    else {
        if ((actualScale < 1e1)) {
            ref_length = (screen_width / 2) * (1e0 / actualScale);
            myDrawList->AddText(ImVec2((screen_width - 75) - (ref_length / 2) - 50, screen_height - 80), ImColor(1.0f, 1.0f, 1.0f, 1.0f), "1 mm");
        }
        else if ((actualScale < 1e2)) {
            ref_length = (screen_width / 2) * (1e1 / actualScale);
            myDrawList->AddText(ImVec2((screen_width - 70) - (ref_length / 2) - 50, screen_height - 80), ImColor(1.0f, 1.0f, 1.0f, 1.0f), "1 cm");
        }
        else if ((actualScale < 1e3)) {
            ref_length = (screen_width / 2) * (1e2 / actualScale);
            myDrawList->AddText(ImVec2((screen_width - 70) - (ref_length / 2) - 50, screen_height - 80), ImColor(1.0f, 1.0f, 1.0f, 1.0f), "10 cm");
        }
        else if ((actualScale < 1e4)) {
            ref_length = (screen_width / 2) * (1e3 / actualScale);
            myDrawList->AddText(ImVec2((screen_width - 70) - (ref_length / 2) - 50, screen_height - 80), ImColor(1.0f, 1.0f, 1.0f, 1.0f), "1 m");
        }
        else if ((actualScale < 1e5)) {
            ref_length = (screen_width / 2) * (1e4 / actualScale);
            myDrawList->AddText(ImVec2((screen_width - 70) - (ref_length / 2) - 50, screen_height - 80), ImColor(1.0f, 1.0f, 1.0f, 1.0f), "10 m");
        }
        else {
            ref_length = (screen_width / 2) * (1e5 / actualScale);
            myDrawList->AddText(ImVec2((screen_width - 70) - (ref_length / 2) - 50, screen_height - 80), ImColor(1.0f, 1.0f, 1.0f, 1.0f), "100 m");
        }
    }
    return ref_length;
}