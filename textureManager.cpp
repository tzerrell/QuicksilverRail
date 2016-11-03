/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   textureManager.cpp
 * Author: tnc02_000
 * 
 * Created on November 3, 2016, 2:21 PM
 */

#include <QImage>
#include <iostream>
#include <string>
#include <map>

#include "textureManager.h"
#include "terrain.h"

std::map<terrain,std::string> textureManager::terrainTextureFilename;
bool textureManager::staticInitializationComplete = false;
void textureManager::initializeStatics() {
    //TODO: Change these from being hardcoded to a file somewhere? ...
    terrainTextureFilename.insert(std::pair<terrain,std::string>(
            terrain::Plains, ":/terrPlains.png"));
    terrainTextureFilename.insert(std::pair<terrain,std::string>(
            terrain::Mountains, ":/terrMountains.png"));
    terrainTextureFilename.insert(std::pair<terrain,std::string>(
            terrain::Swamp, ":/terrSwamp.png"));
    //TODO: Finish
}

textureManager::textureManager() {
}

textureManager::~textureManager() {
}

QOpenGLTexture* textureManager::addTextureFromFile(std::string filename) {
    QImage image(filename);
    if (image.format() == QImage::FORMAT_INVALID) {
        std::cerr << "Failed to load texture \"" << filename "\"\n";
        return nullptr;
    }
    
    texture.emplace_back(image);
    texture.back().setMinificationFilter(QOpenGLTexture::Linear); //TODO: Adjust once I have mipmaps
    texture.back().setMagnificationFilter(QOpenGLTexture::Linear);
    return &(texture.back());
}

QOpenGLTexture* textureManager::getTerrainTexture(terrain t) {
    if (terrainTexture.find(t) == terrainTexture.end()) {
        QOpenGLTexture* tex = addTextureFromFile(terrainTextureFilename[t])
        if(tex == nullptr)
            return nullptr;
        terrainTexture.insert(std::pair(t,tex));
    }
    return terrainTexture[t]; //return texture
}