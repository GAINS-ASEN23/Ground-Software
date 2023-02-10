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
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// global variables for mouse control
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// sense if the mouse is over the gui
bool down;

// define utility functions
unsigned int loadTexture(unsigned char* image_data, int width, int height, int nrChannels, unsigned int texture);

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

    // Generate line shader program from the shader class
    shader lineShaderProgram(vs_line_source, fs_line_source);

    // --- Create the Shaders ---
    unsigned int VBO[2], VAO[2], EBO[1];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(1, EBO);

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

    // --- Create the Gains texture ---
    stbi_set_flip_vertically_on_load(true); // flips images loaded so that 0,0 is on the bottom left corner
    unsigned int textures[3];
    glGenTextures(3, textures); // first input is number of textures to generate, 2nd is pointer to texture / texture array
    int width, height, nrChannels;
    unsigned char* gains_image_data = stbi_load("GAINS_Small_White.png", &width, &height, &nrChannels, 0);
    textures[0] = loadTexture(gains_image_data, width, height, nrChannels, textures[0]);

    // --- Create the Earth texture ---
    stbi_set_flip_vertically_on_load(false);
    unsigned char* earth_image_data = stbi_load("earth2048.bmp", &width, &height, &nrChannels, 0);
    //std::cout << "Earth File Details:" << std::endl << earth_width << std::endl << earth_height << std::endl << earth_nrChannels << std::endl; // for debugging only
    textures[1] = loadTexture(earth_image_data, width, height, nrChannels, textures[1]);

    // --- Create the Moon texture ---
    unsigned char* moon_image_data = stbi_load("moon1024.bmp", &width, &height, &nrChannels, 0);
    textures[2] = loadTexture(moon_image_data, width, height, nrChannels, textures[2]);


    // setup line vertices
    float lineVert[] = {
        -1.0f, -1.0f, 0.0f, // 11
        -0.8f, -1.0f, 0.0f,
        -0.6f, -1.0f, 0.0f,
        -0.4f, -1.0f, 0.0f,
        -0.2f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.2f, -1.0f, 0.0f,
        0.4f, -1.0f, 0.0f,
        0.6f, -1.0f, 0.0f,
        0.8f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,

        1.0f, -0.8f, 0.0f, // 10
        1.0f, -0.6f, 0.0f,
        1.0f, -0.4f, 0.0f,
        1.0f, -0.2f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.2f, 0.0f,
        1.0f, 0.4f, 0.0f,
        1.0f, 0.6f, 0.0f,
        1.0f, 0.8f, 0.0f,
        1.0f, 1.0f, 0.0f,
        
        0.8f, 1.0f, 0.0f, // 10
        0.6f, 1.0f, 0.0f,
        0.4f, 1.0f, 0.0f,
        0.2f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        -0.2f, 1.0f, 0.0f,
        -0.4f, 1.0f, 0.0f,
        -0.6f, 1.0f, 0.0f,
        -0.8f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,

        -1.0f, 0.8f, 0.0f, // 9
        -1.0f, 0.6f, 0.0f,
        -1.0f, 0.4f, 0.0f,
        -1.0f, 0.2f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, -0.2f, 0.0f,
        -1.0f, -0.4f, 0.0f,
        -1.0f, -0.6f, 0.0f,
        -1.0f, -0.8f, 0.0f,

    };

    // Define the pointers to the transformation matrices
    glm::mat4 trans1;
    glm::mat4 trans2;
    glm::mat4 init_trans2;
    glm::mat4 trans3;
    glm::mat4 init_trans3;

    // Define variables for use in loop
    float rotation = 0.0f;
    float translation[] = { -1.0f, 0.0f, 0.0f };
    float earth_rotation = 0.0f;
    float moon_rotation = 0.0f;
    float moon_translation[] = { 1.0f, 0.0f, 0.0f };
    float trajectory_translation[] = { 0.8f, 0.0f, 0.0f };
    bool lock_motion = false;
    float viewScale = 1;
    int lineCount = 10;
    float tempVert[30];
    int step = 0;

    // Create the model,view, and projection transformation matrices
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection;

    // camera directions
    glm::vec3 direction;
    yaw = -90.0f;

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

    // initialize the spice object  
    SPICE spiceFront;
    std::vector<std::vector<double>>PosVectorMoon2;
    //std::cout << "\n XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX \n" << std::endl;
    PosVectorMoon2 = spiceFront.SpiceCall(date, Spice::ObjectID::MOON, Spice::FrameID::J2000, Spice::ObjectID::EARTH, Spice::AbCorrectionID::NONE);
    spiceFront.printSpiceData(PosVectorMoon2);
    //std::cout << "\n OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO \n" << std::endl;
    

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
        ImGui::SetNextWindowPos({ 0,0 }, ImGuiCond_Once);
        ImGui::SetNextWindowSize({ 400,160 }, ImGuiCond_Once);

        // keep track of how long it takes to render each frame
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // adjust the users view
        view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

        // update the uniform color
        float timeValue = glfwGetTime();

        // calculate the correct Planet position and rotation values
        if (lock_motion == false) {
            earth_rotation = fmod((360.0f / 24.0f) * 2 * timeValue,360);
            moon_rotation = fmod((360.0f / 28.0f) * 2 * timeValue,360);
            moon_translation[0] = (cos((2 * float(M_PI) / 28.0f) * 2 * timeValue));
            moon_translation[1] = (sin((2 * float(M_PI) / 28.0f) * 2 * timeValue));
            trajectory_translation[0] = cos((2 * float(M_PI) / 28.0f) * 2 * timeValue) - 0.2 * cos((2 * float(M_PI) / 12.0f) * 2 * timeValue);
            trajectory_translation[1] = sin((2 * float(M_PI) / 28.0f) * 2 * timeValue) - 0.2 * sin((2 * float(M_PI) / 12.0f) * 2 * timeValue);
        }
        
        // --- Draw the Earth ---
        planetShaderProgram.use();
        int modelLoc = glGetUniformLocation(planetShaderProgram.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        int viewLoc = glGetUniformLocation(planetShaderProgram.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projectionLoc = glGetUniformLocation(planetShaderProgram.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        init_trans3 = glm::mat4(1.0f); // this may not be needed any longer
        planetShaderProgram.setMat4("init_trans", init_trans3);
        trans3 = glm::mat4(1.0f);
        trans3 = glm::translate(trans3, (1/viewScale) * glm::vec3(0.0f, 0.0f, 0.0f));
        trans3 = glm::rotate(trans3, glm::radians(earth_rotation), glm::vec3(0.0, 0.0, 1.0));
        trans3 = glm::scale(trans3, (1 / viewScale) * glm::vec3(0.5, 0.5, 0.5));
        planetShaderProgram.setMat4("transform", trans3);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glBindVertexArray(VAO[1]);
        glDrawElements(GL_TRIANGLES, planet.getIndexCount(), GL_UNSIGNED_INT,(void*)0);

        // --- Draw the Moon ---
        planetShaderProgram.use();
        modelLoc = glGetUniformLocation(planetShaderProgram.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        viewLoc = glGetUniformLocation(planetShaderProgram.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        projectionLoc = glGetUniformLocation(planetShaderProgram.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        init_trans3 = glm::mat4(1.0f); // this may not be needed any longer
        planetShaderProgram.setMat4("init_trans", init_trans3);
        trans3 = glm::mat4(1.0f);
        trans3 = glm::translate(trans3, (1/viewScale) * glm::vec3(1.5f*moon_translation[0],1.5f*moon_translation[1],moon_translation[2]));
        //trans3 = glm::translate(trans3, glm::vec3(1.5f * cos((2*float(M_PI) / 28.0f) * 2 * timeValue), 1.5f * sin((2*float(M_PI) / 28.0f) * 2 * timeValue), 0.0f));
        trans3 = glm::rotate(trans3, glm::radians(moon_rotation), glm::vec3(0.0, 0.0, 1.0));
        trans3 = glm::scale(trans3, (1 / viewScale) * glm::vec3(0.125, 0.125, 0.125));
        planetShaderProgram.setMat4("transform", trans3);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[2]);
        glBindVertexArray(VAO[1]);
        glDrawElements(GL_TRIANGLES, planet.getIndexCount(), GL_UNSIGNED_INT, (void*)0);

        
        // Calculate the current points to show on the 2D line
        step++;
        int currentStep = int(std::floor(timeValue * 10)) % (std::size(lineVert) / 3);
        int temp;
        for (int i = 0; i < 3 * lineCount; i++) {
            temp = i + currentStep * 3;
            if (temp < (std::size(lineVert))) {
                tempVert[i] = lineVert[temp];
            }
            else {
                temp = temp - std::size(lineVert);
                tempVert[i] = lineVert[temp];
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
        lineShaderProgram.setVec3("color", glm::vec3(0.8,0.0,0.8));
        trans3 = glm::mat4(1.0f);
        trans3 = glm::translate(trans3, (1 / viewScale) * glm::vec3(1.0,0.0,0.0));
        trans3 = glm::scale(trans3, (1 / viewScale) * glm::vec3(1, 1, 1));
        lineShaderProgram.setMat4("transform", trans3);
        glDrawArrays(GL_LINE_STRIP, 0, lineCount);
        

        // render the --- GUI --- (Design the GUI here)
        ImGui::Begin("GUI Window"); // creates the GUI and names it
        ImGui::Button("Earth Centered ImGui Example Window");
        ImGui::Text("Planetary Distances Are Not To Scale"); // adds a text line to the GUI
        ImGui::Checkbox("Lock Planet Movement",&lock_motion);
        ImGui::SliderFloat("Scale", &viewScale, 0.1, 10);
        ImGui::SliderFloat("Earth Rotation", &earth_rotation, 0, 360);
        ImGui::SliderFloat3("Moon Position", moon_translation, -1.0, 1.0);
        ImGui::SliderFloat("Moon Rotation", &moon_rotation, 0, 360);

        ImGui::End();

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll for IO events (keys pressed/released, mouse moved etc.) - Move from back buffer to front buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // stop rendering ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // deallocate the VAOs and VBOs
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);

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

            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos;
            lastX = xpos;
            lastY = ypos;

            float sensitivity = 0.1f;
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

