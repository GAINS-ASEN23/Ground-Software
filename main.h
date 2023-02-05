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
// Simple 2D Line Shader
const char* vs_line_source = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform mat4 transform;\n"
"uniform vec3 color;\n"
"out vec3 chosenColor;\n" //color from triangle vertex colors
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

// -------------------------
// Simple 2D Shaders
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"uniform float brightness;\n" //value set in the main while loop
"uniform mat4 transform;\n" // multiply the image coordinates by a transformation matrix
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec3 chosenColor;\n" //color from triangle vertex colors
"void main()\n"
"{\n"
//"   gl_Position = transform * vec4(aPos, 1.0);\n"
"   gl_Position = projection * view * model * transform * vec4(aPos, 1.0);\n" //don't forget that matrix multiplication goes from right to left
"   chosenColor = aColor * brightness;\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 chosenColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(chosenColor, 1.0);\n"
"}\n\0";

// Simple 2D Shaders with Textures
const char* vs_Texture_Source = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform float brightness;\n" //color strength value to pass on
"uniform mat4 transform;\n" /* multiply the image coordinates by a transformation matrix */
"uniform mat4 init_trans;\n" /* multiply the image coordinates by a transformation matrix */
"out vec3 chosenColor;\n" 
"out vec2 texCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * transform * init_trans * vec4(aPos, 1.0);\n"
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


// Simple 2D Shaders with Textures
const char* vs_Planet_Source = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform mat4 transform;\n" /* multiply the image coordinates by a transformation matrix */
"uniform mat4 init_trans;\n" /* multiply the image coordinates by a transformation matrix */
"out vec3 chosenColor;\n"
"out vec2 texCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * transform * init_trans * vec4(aPos, 1.0);\n"
"   chosenColor = aNormal;\n"
"   texCoord = aTexCoord * vec2(1,-1);\n" // for some reason the spherical shell generates texture coords flipped vertically so we fix it here
"}\0";

const char* fs_Planet_Source = "#version 330 core\n"
"in vec3 chosenColor;\n"
"in vec2 texCoord;\n"
"out vec4 FragColor;\n"
"uniform sampler2D texture1;\n" //texture to use
"void main()\n"
"{\n"
"   FragColor = texture(texture1,texCoord);\n"
"}\n\0";


// -------------------------


