#ifndef SHADER
#define SHADER
//needed for Shader
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#define WORKING_DIR_FILE(X) ("/home/octavio/random/pong-sdl/pong-sdl/" X)
extern "C"{
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glu.h>
}

class Shader
{
public:
        // The program ID
        GLuint Program;
        GLuint vertex, fragment;
        // Constructor reads and builds the shader
        Shader(const GLchar* vertexSourcePath, const GLchar* fragmentSourcePath);
        // Use the program
        void Use();
        ~Shader();
};
#endif // SHADER

