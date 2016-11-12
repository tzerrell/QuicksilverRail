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

location* location::getNeighbor(direction towardDir) {
    int nbX = x; int nbY = y;
    switch(towardDir) {
        case (direction::E):
            nbX++;
            break;
        case (direction::NE):
            if (y % 2 == 0)
                nbX++;
            nbY++;
            break;
        case (direction::NW):
            if (y % 2 == 1)
                nbX--;
            nbY++;
            break;
        case (direction::W):
            nbX--;
            break;
        case (direction::SW):
            if (y % 2 == 1)
                nbX--;
            nbY--;
            break;
        case (direction::SE):
            if (y % 2 == 0)
                nbX++;
            nbY--;
            break;
    }
    return parent->getLocation(nbX, nbY);
}
