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

#include "board.h"
#include "terrain.h"
#include "connection.h"
#include "connectionEnums.h"

enum class direction;
enum class good;
class settlement;
class player;

class location {
public:
    location();
    location(const location& orig);
    virtual ~location();
    
    void setOwner(board* p) { coordinates.owner = p; }
    board::coord getCoord() { return coordinates; };
    terrain getTerrain() { return terr; }
    void setTerrain(terrain t) { terr = t; }
    connection* getConnection(direction towardDir) { return &(edges.at(towardDir)); };
    void setConnection(direction towardDir, crossing_t crossing, 
            track_t t = track_t::none, player* trackOwner = nullptr,
            bool overwrite = false);
    location* getNeighbor(direction towardDir);
    bool neighborExists(direction towardDir);
    
    void setPosition(int first, int second, 
            board::coord::system s = board::coord::system::orthoLattice) 
        { coordinates.set(first, second, s); }
    void setPosition(float first, float second, 
            board::coord::system s = board::coord::system::ortho) 
        { coordinates.set(first, second, s); }
private:
    board::coord coordinates;
    terrain terr;
    std::map<direction, connection> edges;  //this owns the connections in the E, NE, NW directions. Other three directions are owned by neighbors
    std::vector<good> availableGoods;   //TODO: may be the wrong way to store this. Bitfield?
    settlement* localSettlement;
    
    board::coord positionToward(direction dir);
};

#endif /* LOCATION_H */

