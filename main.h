#pragma once
/* MAIN FRONTEND HEADER FILE
* GAINS Senior Project Capstone Ground Software GUI Frontend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main header file for the Orbital Trajectory Estimation Software
*/

// Assign window height and width
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Define Constants
# define M_PI           3.14159265358979323846  /* pi */ //This is normally defined in math.h, but I will define it here since this is all I need

// Create the Shader Sources
// -------------------------
// Simple Shaders
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"uniform float brightness;\n" //value set in the main while loop
"uniform float rotation;\n" //value set in the main while loop
"out vec3 chosenColor;\n" //color from triangle vertex colors
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x * cos(rotation), aPos.y * sin(rotation), aPos.z, 1.0);\n"
"   chosenColor = aColor * brightness;\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 chosenColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(chosenColor, 1.0);\n"
"}\n\0";

// Simple Shaders with Textures
const char* vs_Texture_Source = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"uniform float brightness;\n" //color strength value to pass on
"uniform float transform_x;\n" // transform the image horizontally
"uniform float transform_y;\n" // transform the image horizontally
"out vec3 chosenColor;\n" 
"out vec2 texCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x+transform_x, aPos.y+transform_y, aPos.z, 1.0);\n"
"   chosenColor = aColor * brightness;\n"
"   texCoord = aTexCoord;\n"
"}\0";

const char* fs_Texture_Source = "#version 330 core\n"
"in vec3 chosenColor;\n"
"in vec2 texCoord;\n"
"out vec4 FragColor;\n"
"uniform sampler2D texture1;\n" //texture to use
"void main()\n"
"{\n"
"   FragColor = texture(texture1,texCoord) * vec4(chosenColor,1.0);\n"
"}\n\0";

// -------------------------


