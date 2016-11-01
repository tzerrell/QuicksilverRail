/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   graphicsResourceManager.cpp
 * Author: tnc02_000
 * 
 * Created on November 1, 2016, 10:12 AM
 */

#include "graphicsResourceManager.h"

#include <fstream>
#include <iostream>
#include <sstream>

graphicsResourceManager::graphicsResourceManager() {
}

graphicsResourceManager::~graphicsResourceManager() {
}

bool graphicsResourceManager::updateShaderFiles(std::string vertFile, std::string fragFile) {
    vertexShaderFilename = vertFile;
    fragmentShaderFilename = fragFile;
    return createShaderProgram();
}

bool graphicsResourceManager::createShaderProgram() {
    std::ostringstream vertShaderCode;
    std::ostringstream fragShaderCode;
    std::ifstream fin;
    
    if (verbose) {
        std::cout << "Creating GLSL shader program from vertex shader file "
                << vertexShaderFilename << " and fragment shader file "
                << fragmentShaderFilename << '\n';
    }
    
    //Read shader code files into strings
    fin.open(vertexShaderFilename);
    if(fin.is_open()) {
        vertShaderCode << fin.rdbuf();
    }
    else {
        std::cerr << "Unable to open vertex shader file "
                << vertexShaderFilename << ". Shader program not created.\n";
        return false;
    }
    fin.close();
    
    fin.open(fragmentShaderFilename);
    if(fin.is_open()) {
        fragShaderCode << fin.rdbuf();
    }
    else {
        std::cerr << "Unable to open fragment shader file "
                << vertexShaderFilename << ". Shader program not created.\n";
        return false;
    }
    fin.close();
    
    //Compile Shaders
    GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    int infoLogLength;
    std::string errorMessage;
    
    if (verbose) std::cout << "Compiling vertex shader\n";
    const char* vertCodePtr = vertShaderCode.str().c_str();
    glShaderSource(vertShaderID, 1, &vertCodePtr, NULL);    //yes, the & is correct, this requires an array of const char *
    glCompileShader(vertShaderID);
    glGetShaderiv(vertShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if ( infoLogLength > 0 ){
        try {
            errorMessage.reserve(infoLogLength+1);
        }
        catch (const std::length_error& ex) {
            std::cerr << "Vertex shader compilation error. Shader program "
                    << "not created. Shader compiler error log too long to "
                    << "output.\n";
            std::cerr << "Length error in handling previous error: " 
                    << ex.what() << '\n';
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        catch (const std::bad_alloc& ex) {
            std::cerr << "Vertex shader compilation error. Shader program "
                    << "not created. Shader compiler error log too long to "
                    << "output.\n";
            std::cerr << "Bad allocation in handling previous error: " 
                    << ex.what() << '\n';
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        catch (...) {
            std::cerr << "Vertex shader compilation error. Shader program not "
                    << "created. Unexpected exception while handling error.\n";
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        glGetShaderInfoLog(vertShaderID, infoLogLength, NULL, &errorMessage[0]);
        std::cerr << "Vertex shader compilation error. Shader program "
                << "not created. Shader compiler error log follows:\n"
                << errorMessage;
        //cleanup
        glDeleteShader(vertShaderID);
        glDeleteShader(fragShaderID);
        return false;
    }
    
    if (verbose) std::cout << "Compiling fragment shader\n";
    const char* fragCodePtr = fragShaderCode.str().c_str();
    glShaderSource(fragShaderID, 1, &fragCodePtr, NULL);    //yes, the & is correct, this requires an array of const char *
    glCompileShader(fragShaderID);
    glGetShaderiv(fragShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if ( infoLogLength > 0 ){
        try {
            errorMessage.reserve(infoLogLength+1);
        }
        catch (const std::length_error& ex) {
            std::cerr << "Fragment shader compilation error. Shader program "
                    << "not created. Shader compiler error log too long to "
                    << "output.\n";
            std::cerr << "Length error in handling previous error: " 
                    << ex.what() << '\n';
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        catch (const std::bad_alloc& ex) {
            std::cerr << "Fragment shader compilation error. Shader program "
                    << "not created. Shader compiler error log too long to "
                    << "output.\n";
            std::cerr << "Bad allocation in handling previous error: " 
                    << ex.what() << '\n';
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        catch (...) {
            std::cerr << "Fragment shader compilation error. Shader program not "
                    << "created. Unexpected exception while handling error.\n";
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        glGetShaderInfoLog(fragShaderID, infoLogLength, NULL, &errorMessage[0]);
        std::cerr << "Fragment shader compilation error. Shader program "
                << "not created. Shader compiler error log follows:\n"
                << errorMessage;            
        //cleanup
        glDeleteShader(vertShaderID);
        glDeleteShader(fragShaderID);
        return false;
    }
    
    //Link GLSL program
    if (verbose) std::cout << "Linking GLSL shader program\n";
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertShaderID);
    glAttachShader(programID, fragShaderID);
    glLinkProgram(programID);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if ( infoLogLength > 0 ){
        try {
            errorMessage.reserve(infoLogLength+1);
        }
        catch (const std::length_error& ex) {
            std::cerr << "GLSL linker error. Shader program not "
                    << "not created. Shader compiler error log too long to "
                    << "output.\n";
            std::cerr << "Length error in handling previous error: " 
                    << ex.what() << '\n';
            glDetachShader(programID, vertShaderID);
            glDetachShader(programID, fragShaderID);
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        catch (const std::bad_alloc& ex) {
            std::cerr << "GLSL linker error. Shader program not "
                    << "not created. Shader compiler error log too long to "
                    << "output.\n";
            std::cerr << "Bad allocation in handling previous error: " 
                    << ex.what() << '\n';
            glDetachShader(programID, vertShaderID);
            glDetachShader(programID, fragShaderID);
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        catch (...) {
            std::cerr << "GLSL linker error. Shader program not "
                    << "created. Unexpected exception while handling error.\n";            
            glDetachShader(programID, vertShaderID);
            glDetachShader(programID, fragShaderID);
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        glGetProgramInfoLog(programID, infoLogLength, NULL, &errorMessage[0]);
        std::cerr << "GLSL linker error. Shader program not created. Error "
                << "log follows:\n"
                << errorMessage;
        
        //cleanup
        glDetachShader(programID, vertShaderID);
        glDetachShader(programID, fragShaderID);
        glDeleteShader(vertShaderID);
        glDeleteShader(fragShaderID);
        return false;
    }
    
    //cleanup
    glDetachShader(programID, vertShaderID);
    glDetachShader(programID, fragShaderID);
    glDeleteShader(vertShaderID);
    glDeleteShader(fragShaderID);
    glDeleteProgram(programID);  //Delete old shader program (if no previous program, this does nothing, which is correct)
    
    //Set shader program to be what we just created
    shaderProgramID = programID;
    
    return true;
}
