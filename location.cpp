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
        : coordinates(0, 0, (board*)nullptr)
        , terr(EnumTraits<terrain>::FIRST)
        , localSettlement(nullptr)
{

}

location::location(const location& orig) 
        : coordinates(orig.coordinates)
{
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
            break;
        case (direction::W):
        case (direction::SW):
        case (direction::SE):
            getNeighbor(towardDir)->setConnection(-towardDir, crossing, t, 
                    trackOwner, overwrite);
    }
}

board::coord location::positionToward(direction dir) {
    board::coord ret(coordinates);
    switch(dir) {
        case (direction::E):
            ret.x += 1;
            break;
        case (direction::NE):
            ret.x += 0.5;
            ret.y += 1;
            break;
        case (direction::NW):
            ret.x -= 0.5;
            ret.y += 1;
            break;
        case (direction::W):
            ret.x -= 1;
            break;
        case (direction::SW):
            ret.x -= 0.5;
            ret.y -= 1;
            break;
        case (direction::SE):
            ret.x += 0.5;
            ret.y -= 1;
            break;
        default:
            throw(std::out_of_range("Invalid direction in positionToward()"));
    }
    return ret;
}

bool location::neighborExists(direction towardDir) {
    return coordinates.owner->isOnBoard(positionToward(towardDir));
}

location* location::getNeighbor(direction towardDir) {
    return coordinates.owner->getLocation(positionToward(towardDir));
}
