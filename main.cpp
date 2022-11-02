/* MAIN FRONTEND FILE
* GAINS Senior Project Capstone Ground Software GUI Frontend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main file for the Orbital Trajectory Estimation Software
*/

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <main.h>

#include <shader.h>

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


    // openGL options
    glEnable(GL_DEPTH_TEST); // depth testing to ensure the proper order of objects

    // Generate shader program from the shader class
    shader shaderProgram(vertexShaderSource, fragmentShaderSource);

    // Generate shader program with textures from the shader class
    shader textureShaderProgram(vs_Texture_Source, fs_Texture_Source);

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


    // --- Create the Shaders ---
    unsigned int VBO[4], VAO[4], EBO[2]; // creates 3 VBOs and VAOs, and makes an EBO
    glGenVertexArrays(4, VAO);
    glGenBuffers(4, VBO);
    glGenBuffers(2, EBO); // EBOs, useful for not needing to declare a vertex multiple times when we make many triangles

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

    // --- Create a texture ---
    stbi_set_flip_vertically_on_load(true); // flips images loaded so that 0,0 is on the bottom left corner
    // Import GAINS Image for use as a texture
    int width, height, nrChannels;
    //unsigned char* GAINS_Image_data = stbi_load("GAINS_Small_Transparent.png", &width, &height, &nrChannels, 0);
    unsigned char* GAINS_Image_data = stbi_load("GAINS_Small_White.png", &width, &height, &nrChannels, 0); 
    //std::cout << stbi_failure_reason() << std::endl; // vug: can't fopen the image
    //unsigned char* GAINS_Image_data = stbi_load("GAINS_Small_Transparent.png", &width, &height, &nrChannels, 0);
    //std::cout << width << std::endl; // bug: this number is becoming negative somehow
    //std::cout << height << std::endl;
    //std::cout << nrChannels << std::endl;
    //unsigned char* GAINS_Image_data = NULL;
    //width = 100;
    //height = 100;
    //nrChannels = 3;
    

    // create the texture
    unsigned int gains_texture;
    glGenTextures(1, &gains_texture); // first input is number of textures to generate, 2nd is pointer to texture / texture array

    glBindTexture(GL_TEXTURE_2D, gains_texture);

    // set texture options for wrapping and filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // can be LINEAR or NEAREST

    // load and generate the texture
    if (GAINS_Image_data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, GAINS_Image_data); // Inputs: 1st specifies texture target,
        // 2nd is mipmap level, 3rd is openGL image format (default = GL_RGB), 4th is width, 5th is height, 6th must be 0, 7th is format,
        // 8th is data type, 9th is the actual image data
        // use GL_RGBA if file uses an alpha (transparency) channel like .pngs
        glGenerateMipmap(GL_TEXTURE_2D); // Mipmaps are very important if we are putting textures on large objects far away
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(GAINS_Image_data); // Frees the image memory now that a texture has been made

    // will need these lines if we use multiple textures in 1 shader program. Repeat the .setInt per texture we use
    //textureShaderProgram.use();
    //textureShaderProgram.setInt("texture1", 0); // assign which textures in main will line up with textures in the shader program

    // Define the pointers to the transformation matrices
    glm::mat4 trans1;
    glm::mat4 trans2;

    // Create the model,view, and projection transformation matrices
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 view = glm::mat4(1.0f);
        // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Read inputs
        processInput(window);

        // Put into back buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
        float colorStrength = sin(timeValue) / 2.0f + 0.5f;
        /*shaderProgram.setFloat("brightness", colorStrength);
        // update the uniform transformation
        trans1 = glm::mat4(1.0f);
        trans1 = glm::scale(trans1, glm::vec3(0.75)*(sin(timeValue/2) / 2.0f + 1.0f));
        shaderProgram.setMat4("transform", trans1);

        // draw the large triangle
        glBindVertexArray(VAO[0]); 
        glDrawArrays(GL_TRIANGLES, 0, 3); //3rd input is the number of vertices to draw

        // draw the small upside down triangle
        shaderProgram.setFloat("brightness", 1-colorStrength);
        glBindVertexArray(VAO[1]); 
        glDrawArrays(GL_TRIANGLES, 0, 3); //3rd input is the number of vertices to draw */

        // draw the bottom pyramid
        trans1 = glm::mat4(1.0f);
        trans1 = glm::rotate(trans1, glm::radians(45 * timeValue), glm::vec3(0.0, 0.0, 1.0));
        trans1 = glm::scale(trans1, glm::vec3(1.0));
        shaderProgram.setMat4("transform", trans1);
        shaderProgram.setFloat("brightness", 1);
        glBindVertexArray(VAO[3]);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

        /*// draw the top pyramid
        trans1 = glm::mat4(1.0f);
        trans1 = glm::translate(trans1, glm::vec3(0.0,0.0,0.5));
        trans1 = glm::rotate(trans1, glm::f32(glm::radians(180.0)), glm::vec3(1.0, 0.0, 0.0));
        trans1 = glm::scale(trans1, glm::vec3(0.5));
        shaderProgram.setMat4("transform", trans1);
        shaderProgram.setFloat("brightness", 1);
        glBindVertexArray(VAO[3]);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);*/

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
        trans2 = glm::mat4(1.0f);
        trans2 = glm::rotate(trans2, glm::radians(45*timeValue), glm::vec3(0.0, 0.0, 1.0));
        trans2 = glm::translate(trans2, glm::vec3(1.0f * cos(2*timeValue), -1.0f * sin(2*timeValue), 0.0f));
        trans2 = glm::scale(trans2, glm::vec3(1.0, 0.75, 1.0));
        textureShaderProgram.setMat4("transform", trans2);
        // set the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gains_texture);
        glBindVertexArray(VAO[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        // Swap buffers and poll for IO events (keys pressed/released, mouse moved etc.) - Move from back buffer to front buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // deallocate the VAOs and VBOs
    glDeleteVertexArrays(3, VAO); // deletes 2 VAOs and VBOs
    glDeleteBuffers(3, VBO);

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
}

// Whenever the window size changed this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}