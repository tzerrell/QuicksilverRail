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

#include <QWindow>

#include "board.h"
#include "location.h"
#include "boardWindow.h"

board::board() 
        : rows(7)   //TODO: initialize these in a sensible way
        , columns(4)
        , loc(boost::extents[4 + 1][7])
        , xOffset(0)    //e.g. if origin (logical (0,0)) of board is at global (3,0), use xOffset = 3
        , yOffset(0)
{
    for (int i = 0; i < columns + 1; ++i) {
        for (int j = 0; j < rows; ++j) {
            loc[i][j].setOwner(this);
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
            loc[i][j].setOwner(this);
            loc[i][j].setPosition(i, j);
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

/*
 * board::coord represents a point on the board (which may or may not be a
 * lattice point (i.e. a point with a location)). Various coordinate systems are
 * useful in different circumstances, and board::coord makes it easy to switch
 * between them.
 * 
 * Orthogonal coordinates have the standard basis.
 * Triangular coordinates have the basis with the two vectors point from origin
 * to the vertex to the east, and from origin to the vertex to the northeast.
 * Lattice versions represent the same things, but rounded to the nearest* point
 * in the lattice. (*Not technically nearest under Euclidean metric, but it's a
 * reasonable enough approximation.)
 * 
 * Orthogonal coords are in (x,y)
 * Triangular coords are in (s,t)
 * Orthogonal lattice coords are in (i,j)
 * Triangular lattice coords are in (l,m)
 */

board::coord::coord(float first, float second, board* own, system s)
        :owner(own)
{
    set(first, second, s);
}

board::coord::coord(int first, int second, board* own, system s)
        :owner(own)
{
    set(first, second, s);
}

board::coord::coord(int vx, int vy, boardWindow* win)
        :owner(win->getSubject())
{
    setFromView(vx, vy, win);
}

void board::coord::set(float first, float second, system s) {
    switch (s) {
        case system::ortho:
            setFromOrthogonal(first, second);
            break;
        case system::tri:
            setFromTriangular(first, second);
            break;
        case system::orthoLattice:
            std::cerr << "Warning: Setting orthogonal lattice coordinates "
                    << "using floats\n";
            setFromOrthoLattice(first, second);
            break;
        case system::globalOrthoLattice:
            std::cerr << "Warning: Setting (global) orthogonal lattice "
                    << "coordinates using floats\n";
            setFromOrthoLattice(first, second, true);
            break;
        case system::triLattice:
            std::cerr << "Warning: Setting triangular lattice coordinates "
                    << "using floats\n";
            setFromTriangularLattice(first, second);
            break;
        default:
            std::cerr << "Unexpected coordinate system in board::coord::set; "
                    << "no coordinates set.\n";
    }
}

void board::coord::set(int first, int second, system s) {
    switch (s) {
        case system::ortho:
            std::cerr << "Warning: Setting orthogonal analog coordinates "
                    << "using ints\n";
            setFromOrthogonal(first, second);
            break;
        case system::tri:
            std::cerr << "Warning: Setting triangular analog coordinates "
                    << "using ints\n";
            setFromTriangular(first, second);
            break;
        case system::orthoLattice:
            setFromOrthoLattice(first, second);
            break;
        case system::globalOrthoLattice:
            setFromOrthoLattice(first, second, true);
            break;
        case system::triLattice:
            setFromTriangularLattice(first, second);
            break;
        default:
            std::cerr << "Unexpected coordinate system in board::coord::set; "
                    << "no coordinates set.\n";
    }
}

void board::coord::setFromOrthoLattice(int i, int j, bool global) {
    if (global) {
        i -= owner->xOffset;
        j -= owner->yOffset;
    }
    
    x = i - (1/2.0) * (j % 2);
    y = j;
}

void board::coord::setFromView(int vx, int vy, boardWindow* win) {
    QRectF view = win->getView();
    double percentX = (vx + 0.5)/win->width();
    double percentY = (vy + 0.5)/win->height();
    x = (percentX * (view.width()) + view.left()) / win->getHorizSpacing();
    y = (percentY * (view.height()) + view.top()) / win->getVertSpacing();
    owner = win->getSubject();
}

float board::coord::s() { return x - (y/2.0); }
float board::coord::t() { return y; }

int board::coord::l() {
    float S = s();
    float rounder;
    if (S > 0)
        rounder = 0.5;
    else
        rounder = -0.5;
    return (int)(S + rounder);
}
int board::coord::m() {
    float T = t();
    float rounder;
    if (T > 0)
        rounder = 0.5;
    else
        rounder = -0.5;
    return (int)(T + rounder);
}

int board::coord::i() {
    float M = m();
    
    float Mrounder;
    if (M >= 0)
        Mrounder = 1;
    else
        Mrounder = 1; //Mrounder = 0;
    return (int)((M + Mrounder)/2) + (int)(l());
}
int board::coord::j() {
    return m();
}

location* board::getLocation(board::coord pt) {
    if (!isOnBoard(pt))
        throw std::out_of_range("Requested location from board::getLocation is not on this board.");    //TODO: Improve?
    return &(loc[pt.i()][pt.j()]);
}

bool board::isOnBoard(board::coord pt) {
    int i = pt.i();
    int j = pt.j();
    if (i < 0 || i > getNumCols() 
            || j < 0 || j >= getNumRows()
            || (j % 2 == 0 && i == getNumCols()))
        return false;
    return true;
}

void board::setLocationTerrain(coord pt, terrain t) {
    getLocation(pt)->setTerrain(t);
}
