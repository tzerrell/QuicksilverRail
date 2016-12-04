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
#include "connection.h"
#include "board.h"
#include "direction.h"
#include <QtGui/QOpenGLTexture>

location::location()
        : parent(nullptr)
        , x(0)
        , y(0)
        , terr(EnumTraits<terrain>::FIRST)
        , localSettlement(nullptr)
{
    
}

location::location(const location& orig) {
    parent = orig.parent;
    x = orig.x; y = orig.y;
    terr = orig.terr;
    if (!edges.empty())
        std::cerr << "Warning: Copying location with non-empty edges. This "
                << "connection information will be lost.\n";
    availableGoods = orig.availableGoods;
    localSettlement = orig.localSettlement;
}

location::~location() {
}


void location::setConnection(direction towardDir, crossing_t crossing, 
        track_t t, player* trackOwner,
        bool overwrite) {
    if (!neighborExists(towardDir))
        throw std::out_of_range("Attempted to connect to off-board vertex.");
    switch (towardDir) {
        case (direction::E):
        case (direction::NE):
        case (direction::NW):
            if (edges.find(towardDir) == edges.end()) {
                edges.insert(std::pair<direction, connection>(towardDir,
                        connection(crossing, t, trackOwner)));
            } else {
                if (!overwrite) {
                    std::cerr << "Warning: location::setConnection called with "
                            << "overwrite == false for a connection that "
                            << "already exists. No connection created.\n";
                    return;
                }
                //else overwrite
                edges[towardDir].reset(crossing, t, trackOwner);
            }
        case (direction::W):
        case (direction::SW):
        case (direction::SE):
            getNeighbor(towardDir)->setConnection(-towardDir, crossing, t, 
                    trackOwner, overwrite);
    }
}

board::coord location::positionToward(direction dir) {
    board::coord ret(x,y,parent);
    std::cout << "{Position From: (" << x << "," << y << ") ";
    std::cout << "ie (" << ret.x << "," << ret.y << ") ";
    switch(dir) {
        case (direction::E):
            ret.x += 1;
            break;
        case (direction::NE):
            std::cout << "[NE" << ret.x << "," << ret.y << ";";
            ret.x += 0.5;
            ret.y += 1;
            std::cout << ret.x << "," << ret.y << "]";
            break;
        case (direction::NW):
            ret.x -= 0.5;
            ret.y += 1;
            break;
        case (direction::W):
            ret.x -= 1;
            break;
        case (direction::SW):
            if (ret.j() % 2 == 1)
                ret.x -= 1;
            ret.y -= 1;
            break;
        case (direction::SE):
            if (ret.j() % 2 == 0)
                ret.x += 1;
            ret.y -= 1;
            break;
        default:
            throw(std::out_of_range("Invalid direction in positionToward()"));
    }
    std::cout << "Position To: (" << ret.i() << "," << ret.j() << ") ";
    std::cout << "ie (" << ret.x << "," << ret.y << ")}\n";
    return ret;
}

bool location::neighborExists(direction towardDir) {
    return parent->isOnBoard(positionToward(towardDir));
}

location* location::getNeighbor(direction towardDir) {
    return parent->getLocation(positionToward(towardDir));
}
