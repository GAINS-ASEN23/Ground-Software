#pragma once
/* MAIN FRONTEND HEADER FILE
* GAINS Senior Project Capstone Ground Software GUI Frontend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main header file for the Orbital Trajectory Estimation Software
*/

// Assign window height and width
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Create the Shader Sources
// -------------------------
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

const char* fragmentShader2Source = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.8f, 0.0f, 1.0f);\n"
"}\n\0";

// -------------------------