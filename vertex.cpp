/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   vertex.cpp
 * Author: tnc02_000
 * 
 * Created on October 31, 2016, 7:01 PM
 */

#include "vertex.h"
#include <QtGui/QOpenGLTexture>

std::map<terrain, QOpenGLTexture*> vertex::texture;
std::map<terrain, QString> vertex::textureFilename;
bool vertex::defaultTextureFilenamesInitialized = false;

vertex::vertex() {
    if (!defaultTextureFilenamesInitialized)
        initializeDefaultTextureFilenames();
}

vertex::~vertex() {
}

QOpenGLTexture* vertex::getTexture() { 
    auto it = texture.find(ter);
    if (it== texture.end()) {
        vertex::loadTexture(ter);
        it = texture.find(ter);
    }
    return (*it).second;
}

void vertex::loadTexture(terrain t) {
    //Note: the new and delete in this function correspond to each other. Since
    //texture is static, we don't need to deal with deletion in the dtor. The
    //only place other than here that this could be deleted is in unloadTexture
    
    auto it = texture.find(t);
    if (it != texture.end()) {
        delete (*it).second;
        texture.erase(it);
    }
    
    QOpenGLTexture* newTexture = new QOpenGLTexture(QImage(vertex::textureFilename[t]).mirrored());
    newTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    newTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture[t] = newTexture;
}

void vertex::unloadTexture(terrain t) {
    //This delete corresponds to the new of loadTexture
    
    auto it = texture.find(t);
    if (it != texture.end()) {
        delete (*it).second;
        texture.erase(it);
    }
}

void vertex::changeTextureFile(terrain t, QString newFilename) {
    textureFilename[t] = newFilename;
}

void vertex::initializeDefaultTextureFilenames() {
    //TODO: This is just the first thing that came to mind; change
    textureFilename[terrain::City] = "terrCity.png";
    textureFilename[terrain::Desert] = "terrDesert.png";
    textureFilename[terrain::Forest] = "terrForest.png";
    textureFilename[terrain::Hills] = "terrHills.png";
    textureFilename[terrain::Jungle] = "terrJungle.png";
    textureFilename[terrain::Mountains] = "terrMountains.png";
    textureFilename[terrain::Plains] = "terrPlains.png";
    textureFilename[terrain::Port] = "terrPort.png";
    textureFilename[terrain::Swamp] = "terrSwamp.png";
    textureFilename[terrain::Town] = "terrTown.png";
    
    defaultTextureFilenamesInitialized = true;
}