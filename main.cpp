/* MAIN FRONTEND FILE
* GAINS Senior Project Capstone Ground Software GUI Frontend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main file for the Orbital Trajectory Estimation Software
*/

#include <iostream>

// required files for GUI. Must load before glad
#include "Libraries/include/ImGui/imgui.h"
#include "Libraries/include/ImGui/imgui_impl_glfw.h"
#include "Libraries/include/ImGui/imgui_impl_opengl3.h"
//#include "Libraries/include/ImGui/imgui_impl_opengl3_loader.h" // I am unsure if this is needed as an #include

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <main.h>

#include <shader.h>
#include <Sphere.h>

//#include "BACKEND/data.h" // for testing only

// for image loading and converting to texture functionality
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// for vector and matrix math - ask Derek how to link this
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// BACKEND INCLUDES
#include "BACKEND/mainBackend.h"

// Create functions to outline what happens in window
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


//glfwSetMouseButtonCallback(window, ImGui_ImplGlfw_MouseButtonCallback)

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

// other functions
unsigned int loadTexture(unsigned char* image_data, int width, int height, int nrChannels, unsigned int texture);
void loadShaderObjects(unsigned int* VBO, unsigned int* VAO, unsigned int* EBO);

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

    /* TEMPORARY BACKEND STUFF */
    SPICE spiceOBJ;
    // Test Function from sub files 
    spiceOBJ.printSpiceData();

    // Setup the GUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // openGL options
    glEnable(GL_DEPTH_TEST); // depth testing to ensure the proper order of objects

    // Generate shader program from the shader class
    shader shaderProgram(vertexShaderSource, fragmentShaderSource);

    // Generate shader program with textures from the shader class
    shader textureShaderProgram(vs_Texture_Source, fs_Texture_Source);

    // Generate shader program with textures from the shader class
    shader planetShaderProgram(vs_Planet_Source, fs_Planet_Source);

    // Generate line shader program from the shader class
    shader lineShaderProgram(vs_line_source, fs_line_source);

    // --- Create the Shaders ---
    unsigned int VBO[5], VAO[5], EBO[3]; // creates VBOs, VAOs, and EBOs
    glGenVertexArrays(5, VAO);
    glGenBuffers(5, VBO);
    glGenBuffers(3, EBO); // EBOs, useful for not needing to declare a vertex multiple times when we make many triangles
    //loadShaderObjects(VBO, VAO, EBO); // not needed since we don't currently utilize these objects

    // Generate a sphere
    Sphere planet(1.0f, 36, 18);           // radius, sectors, stacks, smooth(default)
    glBindVertexArray(VAO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, planet.getInterleavedVertexSize(), planet.getInterleavedVertices(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, planet.getIndexSize(), planet.getIndices(), GL_STATIC_DRAW);
    int stride = planet.getInterleavedStride();     // should be 32 bytes
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
    //int earth_width, earth_height, earth_nrChannels;
    unsigned char* earth_image_data = stbi_load("earth2048.bmp", &width, &height, &nrChannels, 0);
    //std::cout << "Earth File Details:" << std::endl << earth_width << std::endl << earth_height << std::endl << earth_nrChannels << std::endl; // for debugging only
    textures[1] = loadTexture(earth_image_data, width, height, nrChannels, textures[1]);

    // --- Create the Moon texture ---
    unsigned char* moon_image_data = stbi_load("moon1024.bmp", &width, &height, &nrChannels, 0);
    textures[2] = loadTexture(moon_image_data, width, height, nrChannels, textures[2]);


    // setup line vertices
    float lineVert[] = {
        /*-0.25f, -0.25f, 0.0f,
        0.0f, 0.25f, 0.0f,
        0.25f, -0.25f, 0.0f,
        -0.35f, 0.10f, 0.0f,
        0.35f, 0.10f, 0.0f,
        - 0.25f, -0.25f, 0.0f,*/

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

    int lineCount = 10;
    float tempVert[30];
    //float tempVert[] = {0.0f, 0.0f, 0.0f};
    int step = 0;

    // Define the pointers to the transformation matrices
    glm::mat4 trans1;
    glm::mat4 trans2;
    glm::mat4 init_trans2;
    glm::mat4 trans3;
    glm::mat4 init_trans3;
    float rotation = 0.0f;
    float translation[] = { -1.0f, 0.0f, 0.0f };
    float earth_rotation = 0.0f;
    float moon_rotation = 0.0f;
    float moon_translation[] = { 1.0f, 0.0f, 0.0f };
    float trajectory_translation[] = { 0.8f, 0.0f, 0.0f };
    bool lock_motion = false;
    float viewScale = 1;

    // Create the model,view, and projection transformation matrices
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection;

    // camera directions
    glm::vec3 direction;
    yaw = -90.0f;

    //glfwSetCursorPosCallback(window, mouse_callback); // captures mouse movements while the cursor is captured
    glfwSetScrollCallback(window, scroll_callback); // captures mouse scroll wheel actions

    // initialize the spice object
    /*
    SPICE spiceFront;
    std::vector<std::vector<double>> PosVectorMoon2;
    //PosVectorMoon2 = spiceFront.SpiceCall(date, Spice::ObjectID::MOON, Spice::FrameID::J2000, Spice::ObjectID::EARTH, Spice::AbCorrectionID::NONE); //why is date undefined?
    spiceFront.printSpiceData();
    */
    

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

        // Capture Mouse Clicks on ImGui
        //ImGuiIO& io = ImGui::GetIO();
        //io.AddMouseButtonEvent(GLFW_MOUSE_BUTTON_LEFT, down);
        //std::cout << (io.WantCaptureMouse) << std::endl;

        // choose the shader program to use
        shaderProgram.use();

        // update the model, view, and projection transformation matrices
        int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projectionLoc = glGetUniformLocation(shaderProgram.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        /*// Draw the rectangles with EBOs
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);*/

        // update the uniform color
        float timeValue = glfwGetTime();

        /*
        // draw the bottom pyramid
        trans1 = glm::mat4(1.0f);
        trans1 = glm::translate(trans1, glm::vec3(translation[0], translation[1], translation[2]));
        //trans1 = glm::rotate(trans1, glm::radians(45 * timeValue), glm::vec3(0.0, 0.0, 1.0));
        trans1 = glm::rotate(trans1, rotation, glm::vec3(0.0, 0.0, 1.0));
        trans1 = glm::scale(trans1, glm::vec3(1.0));
        shaderProgram.setMat4("transform", trans1);
        shaderProgram.setFloat("brightness", 1);
        glBindVertexArray(VAO[3]);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
        */

        /*
        //draw the gains logo
        textureShaderProgram.use();
        textureShaderProgram.setFloat("brightness", 1);
        // update the model, view, and projection transformation matrices
        modelLoc = glGetUniformLocation(textureShaderProgram.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        viewLoc = glGetUniformLocation(textureShaderProgram.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        projectionLoc = glGetUniformLocation(textureShaderProgram.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        //transform it
        init_trans2 = glm::mat4(1.0f);
        init_trans2 = glm::rotate(init_trans2, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        textureShaderProgram.setMat4("init_trans", init_trans2);
        trans2 = glm::mat4(1.0f);
        trans2 = glm::rotate(trans2, glm::radians(45*timeValue), glm::vec3(0.0, 0.0, 1.0));
        trans2 = glm::translate(trans2, glm::vec3(1.0f * cos(2*timeValue), -1.0f * sin(2*timeValue), 0.0f));
        trans2 = glm::scale(trans2, glm::vec3(1.0, 0.75, 1.0));
        textureShaderProgram.setMat4("transform", trans2);
        // set the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glBindVertexArray(VAO[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        */

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
        modelLoc = glGetUniformLocation(planetShaderProgram.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        viewLoc = glGetUniformLocation(planetShaderProgram.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        projectionLoc = glGetUniformLocation(planetShaderProgram.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        init_trans3 = glm::mat4(1.0f);
        //init_trans3 = glm::rotate(init_trans3, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //init_trans3 = glm::scale(init_trans3, glm::vec3(1/viewScale, 1/viewScale, 1/viewScale));
        planetShaderProgram.setMat4("init_trans", init_trans3);
        trans3 = glm::mat4(1.0f);
        trans3 = glm::translate(trans3, (1/viewScale) * glm::vec3(0.0f, 0.0f, 0.0f));
        trans3 = glm::rotate(trans3, glm::radians(earth_rotation), glm::vec3(0.0, 0.0, 1.0));
        trans3 = glm::scale(trans3, (1 / viewScale) * glm::vec3(0.5, 0.5, 0.5));
        planetShaderProgram.setMat4("transform", trans3);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glBindVertexArray(VAO[4]);
        glDrawElements(GL_TRIANGLES, planet.getIndexCount(), GL_UNSIGNED_INT,(void*)0);

        // --- Draw the Moon ---
        planetShaderProgram.use();
        modelLoc = glGetUniformLocation(planetShaderProgram.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        viewLoc = glGetUniformLocation(planetShaderProgram.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        projectionLoc = glGetUniformLocation(planetShaderProgram.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        init_trans3 = glm::mat4(1.0f);
        //init_trans3 = glm::rotate(init_trans3, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //init_trans3 = glm::scale(init_trans3, glm::vec3(1 / viewScale, 1 / viewScale, 1 / viewScale));
        planetShaderProgram.setMat4("init_trans", init_trans3);
        trans3 = glm::mat4(1.0f);
        trans3 = glm::translate(trans3, (1/viewScale) * glm::vec3(1.5f*moon_translation[0],1.5f*moon_translation[1],moon_translation[2]));
        //trans3 = glm::translate(trans3, glm::vec3(1.5f * cos((2*float(M_PI) / 28.0f) * 2 * timeValue), 1.5f * sin((2*float(M_PI) / 28.0f) * 2 * timeValue), 0.0f));
        trans3 = glm::rotate(trans3, glm::radians(moon_rotation), glm::vec3(0.0, 0.0, 1.0));
        trans3 = glm::scale(trans3, (1 / viewScale) * glm::vec3(0.125, 0.125, 0.125));
        planetShaderProgram.setMat4("transform", trans3);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[2]);
        glBindVertexArray(VAO[4]);
        glDrawElements(GL_TRIANGLES, planet.getIndexCount(), GL_UNSIGNED_INT, (void*)0);

        
        // render a 2D line
        step++;
        //std::cout << "------------------------------" << std::endl;
        //int currentStep = int(std::floor(step / 3)) % (std::size(lineVert) / 3);
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
        /*for (int i = 0; i < lineCount; i++) {
            std::cout << tempVert[3*i] << ',' << tempVert[3*i+1] << ',' << tempVert[3*i+2] << std::endl;
        }
        std::cout << currentStep << std::endl;*/

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
        //glBindVertexArray(VAO[0]);
        //glDrawElements(GL_LINES, 3, GL_FLOAT, 0); // does nothing yet
        glDrawArrays(GL_LINE_STRIP, 0, lineCount);
        //glDrawArrays(GL_POINTS, 0, 5);
        

        
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
    glDeleteVertexArrays(5, VAO);
    glDeleteBuffers(5, VBO);

    // delete the shader program just in caes
    shaderProgram.~shader();

    // Clear all previously allocated GLFW resources
    glfwTerminate();
    return 0;
}

// Query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //auto& io = ImGui::GetIO();
    //if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
    //    return;
    //}
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
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

/*void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(GLFW_MOUSE_BUTTON_LEFT, down);
    //io.AddMouseButtonEvent(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS);
    //auto& io = ImGui::GetIO();
    //if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
    //    return;
    //}
    std::cout << (io.WantCaptureMouse) << std::endl;
    if (!io.WantCaptureMouse) {
        //return;

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
}*/

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    //auto& io = ImGui::GetIO();
    //if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
    //    return;
    //}
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f) // 45.0f is the normal limit
        fov = 45.0f;
}

unsigned int loadTexture(unsigned char* image_data, int width, int height, int nrChannels, unsigned int texture) {

    // create the texture
    //unsigned int texture;
    //glGenTextures(1, &texture); // first input is number of textures to generate, 2nd is pointer to texture / texture array
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
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data); // Inputs: 1st specifies texture target,
            // 2nd is mipmap level, 3rd is openGL image format (default = GL_RGB), 4th is width, 5th is height, 6th must be 0, 7th is format,
            // 8th is data type, 9th is the actual image data
            // use GL_RGBA if file uses an alpha (transparency) channel like .pngs
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

void loadShaderObjects(unsigned int* VBO, unsigned int* VAO, unsigned int* EBO) {
    // Create triangle definition matrices
    float colorTriangle[] = {
        // positions         // colors
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };

    float reverseColorTriangle[] = {
        // positions         // colors
     -0.25f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,   // top left
      0.25f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,   // top right
     0.0f,  -0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // bottom
    };

    // Gains Loggo
    float gainsRectangle[] = {
        // positions           // colors        // texture coords
    -0.2f, -0.45f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, // bottom left
    -0.2f, 0.45f,  0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f, // top left
    0.2f,  0.45f,  0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // top right
    0.2f, -0.45f,  0.0f,   0.0f, 0.0f, 0.0f,   1.0f, 0.0f // bottom right
    };
    unsigned int gainsIndices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    // 3D square pyramid
    float pyramid[] = {
        // positions          // colors
     -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 0.8f,   // top left
      0.5f,  0.5f, -0.5f,   0.0f, 0.8f, 0.8f,   // top right
      0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0.8f,   // bottom right
     -0.5f, -0.5f, -0.5f,   0.0f, 0.8f, 0.8f,   // bottom left
      0.0f,  0.0f,  0.5f,   0.0f, 0.0f, 0.0f    // front point
    };
    unsigned int pyramidIndices[] = {
        0, 1, 2,   // first bottom triangle
        0, 2, 3,   // second bottom triangle
        0, 1, 4,   // side triangle 1
        1, 2, 4,   // side triangle 1
        2, 3, 4,   // side triangle 1
        3, 0, 4,   // side triangle 1
    };

    // Initialize 1st shader object (large triangle)
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colorTriangle), colorTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // color attribute
    glEnableVertexAttribArray(1);

    // Initialize 2nd shader object (small upside down triangle)
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(reverseColorTriangle), reverseColorTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // color attribute
    glEnableVertexAttribArray(1);

    // Initialize the 3rd shader object (the gains logo)
    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gainsRectangle), gainsRectangle, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gainsIndices), gainsIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Initialize the pyramid object
    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid), pyramid, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramidIndices), pyramidIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // color attribute
    glEnableVertexAttribArray(1);

}
