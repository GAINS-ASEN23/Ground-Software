#pragma once
/* SHADER CLASS HEADER FILE
* GAINS Senior Project Capstone Ground Software Render Engine Component
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the header file for a flexible shader compiling and linking class
*/

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

class shader
{
public:
    // the program ID
    unsigned int ID;

    // constructor reads and builds the shader
    shader(const char* vertexPath, const char* fragmentPath);
    ~shader();

    // use/activate the shader
    void use();

    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, const GLfloat* value) const;

private:
    // function to check for linking/compile errors
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif