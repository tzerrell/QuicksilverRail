/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   textureManager.h
 * Author: tnc02_000
 *
 * Created on November 3, 2016, 2:21 PM
 */

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <vector>
#include <string>
#include <map>
#include <QtGui/QOpenGLTexture>

enum class terrain; //forward declaration

class textureManager {
public:
    textureManager();
    textureManager(const textureManager&) = delete;
    virtual ~textureManager();
    
    QOpenGLTexture* addTextureFromFile(std::string filename);
    QOpenGLTexture* getTerrainTexture(terrain t);
private:
    std::vector<QOpenGLTexture> texture;
    std::map<terrain,QOpenGLTexture*> terrainTexture;
    static std::map<terrain,std::string> terrainTextureFilename;
    static bool staticInitializationComplete;
    static void initializeStatics();
};

#endif /* TEXTUREMANAGER_H */

