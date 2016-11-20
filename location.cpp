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

location::coordinate location::positionToward(direction dir) {
    coordinate ret;
    ret.x = x; ret.y = y;
    switch(dir) {
        case (direction::E):
            ret.x++;
            break;
        case (direction::NE):
            if (y % 2 == 0)
                ret.x++;
            ret.y++;
            break;
        case (direction::NW):
            if (y % 2 == 1)
                ret.x--;
            ret.y++;
            break;
        case (direction::W):
            ret.x--;
            break;
        case (direction::SW):
            if (y % 2 == 1)
                ret.x--;
            ret.y--;
            break;
        case (direction::SE):
            if (y % 2 == 0)
                ret.x++;
            ret.y--;
            break;
    }
    return ret;
}

bool location::neighborExists(direction towardDir) {
    coordinate coord = positionToward(towardDir);
    return parent->isOnBoard(coord.x, coord.y);
}

location* location::getNeighbor(direction towardDir) {
    coordinate coord = positionToward(towardDir);
    board::coord pt(coord.x, coord.y, parent,
            board::coord::system::orthoLattice);
    return parent->getLocation(pt);
}
