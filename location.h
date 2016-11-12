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

#include <map>

#include <QtGui/QOpenGLTexture>

#include "terrain.h"

enum class direction;
enum class good;
class board;
class connection;
class settlement;

class location {
public:
    location();
    location(const location& orig);
    virtual ~location();
    
    terrain getTerrain() { return terr; }
    void setTerrain(terrain t) { terr = t; }
    connection* getConnection(direction towardDir);
    void setConnection(direction towardDir);
    location* getNeighbor(direction towardDir);
    
    void setPosition(int newX, int newY) { x = newX; y = newY; }
private:
    board* parent;
    int x;  int y;
    terrain terr;
    std::map<direction, connection> edges;
    std::vector<good> availableGoods;   //TODO: may be the wrong way to store this. Bitfield?
    settlement* localSettlement;
};

#endif /* LOCATION_H */

