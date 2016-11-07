/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   location.cpp
 * Author: tnc02_000
 * 
 * Created on October 31, 2016, 7:01 PM
 */

#include "location.h"
#include <QtGui/QOpenGLTexture>

std::map<terrain, QOpenGLTexture*> location::texture;
std::map<terrain, QString> location::textureFilename;
bool location::defaultTextureFilenamesInitialized = false;

location::location() {
    if (!defaultTextureFilenamesInitialized)
        initializeDefaultTextureFilenames();
}

location::~location() {
}

QOpenGLTexture* location::getTexture() { 
    auto it = texture.find(terr);
    if (it== texture.end()) {
        location::loadTexture(terr);
        it = texture.find(terr);
    }
    return (*it).second;
}

void location::loadTexture(terrain t) {
    //Note: the new and delete in this function correspond to each other. Since
    //texture is static, we don't need to deal with deletion in the dtor. The
    //only place other than here that this could be deleted is in unloadTexture
    
    auto it = texture.find(t);
    if (it != texture.end()) {
        delete (*it).second;
        texture.erase(it);
    }
    
    QOpenGLTexture* newTexture = new QOpenGLTexture(QImage(location::textureFilename[t]).mirrored());
    newTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    newTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture[t] = newTexture;
}

void location::unloadTexture(terrain t) {
    //This delete corresponds to the new of loadTexture
    
    auto it = texture.find(t);
    if (it != texture.end()) {
        delete (*it).second;
        texture.erase(it);
    }
}

void location::changeTextureFile(terrain t, QString newFilename) {
    textureFilename[t] = newFilename;
}

void location::initializeDefaultTextureFilenames() {
    //TODO: This is just the first thing that came to mind; change
    textureFilename[terrain::City] = "res/terrCity.png";
    textureFilename[terrain::Desert] = "res/terrDesert.png";
    textureFilename[terrain::Forest] = "res/terrForest.png";
    textureFilename[terrain::Hills] = "res/terrHills.png";
    textureFilename[terrain::Jungle] = "res/terrJungle.png";
    textureFilename[terrain::Mountains] = "res/terrMountains.png";
    textureFilename[terrain::Plains] = "res/terrPlains.png";
    textureFilename[terrain::Port] = "res/terrPort.png";
    textureFilename[terrain::Swamp] = "res/terrSwamp.png";
    textureFilename[terrain::Town] = "res/terrTown.png";
    
    defaultTextureFilenamesInitialized = true;
}