/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   location.h
 * Author: tnc02_000
 *
 * Created on October 31, 2016, 7:01 PM
 */

#ifndef LOCATION_H
#define LOCATION_H

#include <QtGui/QOpenGLTexture>

#include "terrain.h"

class location {
public:
    location();
    location(const location&) = delete;
    virtual ~location();
    
    terrain getTerrain() { return ter; };
    void setTerrain(terrain t) { ter = t; };
    QOpenGLTexture* getTexture();
    
    static void loadTexture(terrain t);  //Loads the texture for terrain t into "texture"
    static void unloadTexture(terrain t); //Unloads the texture for terrain t from "texture"
    static void changeTextureFile(terrain t, QString newFilename);
private:
    static void initializeDefaultTextureFilenames();    //TODO: Needs to be called before class is first used
    
    static bool defaultTextureFilenamesInitialized;
    terrain ter;
    static std::map<terrain, QOpenGLTexture*> texture;
    static std::map<terrain, QString> textureFilename;
};

#endif /* LOCATION_H */

