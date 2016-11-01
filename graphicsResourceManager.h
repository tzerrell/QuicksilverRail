/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   graphicsResourceManager.h
 * Author: tnc02_000
 *
 * Created on November 1, 2016, 10:12 AM
 */

#ifndef GRAPHICSRESOURCEMANAGER_H
#define GRAPHICSRESOURCEMANAGER_H

#include <QtGui/QOpenGLFunctions>

#include <string>

class graphicsResourceManager : protected QOpenGLFunctions {
public:
    graphicsResourceManager();
    graphicsResourceManager(const graphicsResourceManager&) = delete;
    virtual ~graphicsResourceManager();
    
    //updateShader Files returns whether a new shader program was successfully
    //created. If false, the old shader program (if any) is retained in
    //shaderProgramID, but the filenames are still changed to the parameters
    //passed by this function.
    bool updateShaderFiles(std::string vertFile, std::string fragFile); 
private:
    bool createShaderProgram();     //return whether successfully created. If false, the old shader program (if any) is retained in shaderProgramID
    std::string vertexShaderFilename;
    std::string fragmentShaderFilename;
    GLuint shaderProgramID;
    bool verbose;
};

#endif /* GRAPHICSRESOURCEMANAGER_H */

