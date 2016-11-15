/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   board.cpp
 * Author: tnc02_000
 * 
 * Created on October 30, 2016, 10:17 PM
 */

#include <boost/multi_array.hpp>

#include "board.h"
#include "location.h"

board::board() 
        : rows(7)   //TODO: initialize these in a sensible way
        , columns(4)
        , loc(boost::extents[4 + 1][7])
        , xOffset(0)    //e.g. if origin (logical (0,0)) of board is at global (3,0), use xOffset = 3
        , yOffset(0)
{
    for (int i = 0; i < columns + 1; ++i) {
        for (int j = 0; j < rows; ++j) {
            loc[i][j].setParent(this);
            loc[i][j].setPosition(i + xOffset, j + yOffset);
        }
    }
}

board::board(int height, int width)
        : rows(height)
        , columns(width)
        , loc(boost::extents[width + 1][height])
        , xOffset(0)
        , yOffset(0)
{
    for (int i = 0; i < columns + 1; ++i) {
        for (int j = 0; j < rows; ++j) {
            loc[i][j].setParent(this);
        }
    }
}


board::board(std::istream& in) {
    //TODO
}

board::board(const board& orig) {
}

board::~board() {
}

location* board::getLocation(int x, int y, bool logicalCoords) {
    int xIndex = x;
    int yIndex = y;
    if (!logicalCoords) {   //logicalCoords indicates whether to use the coords of the boards loc array (true)
                            //or the global world coordinates (x,y) (false)
        xIndex -= xOffset;
        yIndex -= yOffset;
    }
    if (!isOnBoard(xIndex, yIndex, true))
        throw std::out_of_range("Requested location from board::getLocation is not on this board.");    //TODO: Improve?
    return &(loc[xIndex][yIndex]);
}

bool board::isOnBoard(int x, int y, bool logicalCoords) {
    int xIndex = x;
    int yIndex = y;
    if (!logicalCoords) {   //logicalCoords indicates whether to use the coords of the boards loc array (true)
                            //or the global world coordinates (x,y) (false)
        xIndex -= xOffset;
        yIndex -= yOffset;
    }
    if (xIndex < 0 || xIndex > getNumCols() 
            || yIndex < 0 || yIndex >= getNumRows()
            || (yIndex % 2 == 0 && xIndex == getNumCols()))
        return false;
    return true;
}

void board::setLocationTerrain(int x, int y, terrain t) {
    getLocation(x,y)->setTerrain(t);
}
