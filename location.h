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

class board;

class location {
public:
    location();
    location(const location&) = delete;
    virtual ~location();
    
    terrain getTerrain() { return terr; };
    void setTerrain(terrain t) { terr = t; };
    QOpenGLTexture* getTexture();
    
    static void loadTexture(terrain t);  //Loads the texture for terrain t into "texture"
    static void unloadTexture(terrain t); //Unloads the texture for terrain t from "texture"
    static void changeTextureFile(terrain t, QString newFilename);
private:
    static void initializeDefaultTextureFilenames();    //TODO: Needs to be called before class is first used
    
    board* parent;
    static bool defaultTextureFilenamesInitialized;
    terrain terr;
    //this class is responsible for managing the memory of textures associated
    //with terrain. When entries are added or deleted from 'texture', the memory
    //for them is allocated/deallocated. loadTexture/unloadTexture do this.
    static std::map<terrain, QOpenGLTexture*> texture;
    static std::map<terrain, QString> textureFilename;
};

#endif /* LOCATION_H */

