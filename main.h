#pragma once
/* MAIN FRONTEND HEADER FILE
* GAINS Senior Project Capstone Ground Software GUI Frontend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main header file for the Orbital Trajectory Estimation Software
*/

//----- Includes -----

#include <iostream>

// required files for GUI. Must load before glad
#include "Libraries/include/ImGui/imgui.h"
#include "Libraries/include/ImGui/imgui_impl_glfw.h"
#include "Libraries/include/ImGui/imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shader.h>
#include <shapeData.h>
#include <Sphere.h>

#include <chrono>

// for image loading and converting to texture functionality
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// for vector and matrix math - ask Derek how to link this
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// BACKEND INCLUDES
#include "BACKEND/mainBackend.h"

// Assign window height and width
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Define Constants
# define M_PI           3.14159265358979323846  /* pi */ //This is normally defined in math.h, but I will define it here since this is the only variable I need

//----- Shader Sources -----
// Simple 2D shaders intended for drawing lines
const char* vs_line_source = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform mat4 transform;\n"
"uniform vec3 color;\n"
"out vec3 chosenColor;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * transform * vec4(aPos, 1.0);\n" //don't forget that matrix multiplication goes from right to left
"   chosenColor = color;\n" //vec3(0.8, 0.0, 0.0);\n
"}\0";

const char* fs_line_source = "#version 330 core\n"
"in vec3 chosenColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(chosenColor, 1.0);\n"
"}\n\0";


// 3D shaders for spheres made from "Sphere.h"
const char* vs_Planet_Source = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform mat4 transform;\n"
"out vec3 chosenColor;\n"
"out vec2 texCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * transform * vec4(aPos, 1.0);\n"
"   chosenColor = aNormal;\n"
"   texCoord = aTexCoord;\n"
"}\0";

const char* fs_Planet_Source = "#version 330 core\n"
"in vec3 chosenColor;\n"
"in vec2 texCoord;\n"
"out vec4 FragColor;\n"
"uniform sampler2D texture1;\n"
"void main()\n"
"{\n"
"   FragColor = texture(texture1,texCoord);\n"
"}\n\0";

// 3D shaders for circular icons
const char* vs_Icon_Source = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform mat4 transform;\n"
"uniform vec3 color;\n"
"out vec3 chosenColor;\n"
"out vec2 texCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * transform * vec4(aPos, 1.0);\n"
"   chosenColor = color;\n"
"   texCoord = aTexCoord;\n"
"}\0";

const char* fs_Icon_Source = "#version 330 core\n"
"in vec3 chosenColor;\n"
"in vec2 texCoord;\n"
"out vec4 FragColor;\n"
"uniform sampler2D texture1;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(chosenColor, 1.0);\n"
"}\n\0";

// 3D shaders for textured 3D objects
const char* vs_3d_Source = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform mat4 transform;\n"
"uniform vec3 color;\n"
"out vec3 chosenColor;\n"
"out vec2 texCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * transform * vec4(aPos, 1.0);\n"
"   chosenColor = color;\n"
"   texCoord = aTexCoord;\n"
"}\0";

const char* fs_3d_Source = "#version 330 core\n"
"in vec3 chosenColor;\n"
"in vec2 texCoord;\n"
"out vec4 FragColor;\n"
"uniform sampler2D texture1;\n"
"void main()\n"
"{\n"
"   FragColor = texture(texture1,texCoord);" //vec4(chosenColor, 1.0);\n"
"}\n\0";