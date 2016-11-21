/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   board.h
 * Author: tnc02_000
 *
 * Created on October 30, 2016, 10:17 PM
 */

/*
 * The main board / main map of Quicksilver Rail. Contains both vertices and
 * edges. Vertices contain data on what the terrain is (plains, swamp, town,
 * etc) and edges contain data on who, if anyone, owns the rail in addition to
 * terrain data about crossings (normal land? need to bridge a river? need to
 * bridge an channel/estuary? sea-to-sea edge?)
 */

/*
 * Diagrams
 * 
 * Every edge is related to the vertex to the left and below it (always at least
 * somewhat to the left, never above (but not below for horizontal edges)
 * 
 *       / \
 *      /   \
 *   (0,1) (1,1)
 *    /       \
 *   /         \
 *(0,0)-(1,0)---
 *
 * For a "3x4" board:
 * y 
 * 4    *---*---*---*
 * 3   / \ / \ / \ / \
 * 2  *---*---*---*---*
 * 1   \ / \ / \ / \ /
 * 0    *---*---*---*
 *  x 00112233445566778
 * 
 * Note that a few entries near the edge will be missing, such as (0,0) or (0,4)
 * 
 * If the "dimensions" of the board are m x n, then need 2m-1 rows, 2n+1 columns
 * in the data matrix. Should always use m such that m%2 = 1.
 * 
 */

#ifndef BOARD_H
#define BOARD_H

#include <boost/multi_array.hpp>
#include <iostream>

#include "direction.h"
#include "terrain.h"

class location;
class track;
class player;
class boardWindow;

class board {
public:
    board();
    explicit board(int height, int width);
    explicit board(std::istream& in);
    board(const board& orig);
    virtual ~board();
    
    struct coord {
        float x;
        float y;
        board* owner;
        
        enum class system {
            ortho,
            tri,
            orthoLattice,
            globalOrthoLattice,
            triLattice
        };
        
        coord(float first, float second, board* own, system s = system::ortho);
        coord(int first, int second, board* own, system s = system::orthoLattice);
        coord(int vx, int vy, boardWindow* win);
        coord() : coord(0,0, (board*)nullptr) {};
        void set(float first, float second, system s);
        void set(int first, int second, system s);
        void setFromOrthogonal(float nx, float ny) { x = nx; y = ny; };
        void setFromTriangular(float s, float t) { x = s + t/2; y = t; };
        void setFromOrthoLattice(int i, int j, bool global = false);
        void setFromTriangularLattice(int l, int m) { x = l + m/2; y = m; };
        void setFromView(int vx, int vy, boardWindow* win);
        int i();    int j();    //The orthogonal lattice coordinates of this pt
        int l();    int m();    //The triangular lattice coordinates of this pt
        float s();  float t();  //The triangular analog coordinates of this pt
    };
    
    location* getLocation(coord pt);
    void setLocationTerrain(int x, int y, terrain t);
    crossing* getCrossing(int x, int y, direction d);
    void setCrossing(int x, int y, direction d, crossing c);
    track* getTrack(int x, int y, direction d);
    void setTrack(int x, int y, direction d, track t);
    void resize(int height, int width);
    int getNumRows() { return rows; };
    int getNumCols() { return columns; };
    
    bool isOnBoard(coord pt);
private:
    int rows;
    int columns;    //Note: counts full zig-zag cols; & not half-col at end
    int xOffset;    //How much x differs from the internal col index
    int yOffset;    //How much y differs from the internal row index    //TODO: Important, must be even
    boost::multi_array<location, 2> loc;
    //boost::multi_array<int, 2> landData; //contains terrain on vertices, and
                                         //rails on edges, see diagrams above
};

#endif /* BOARD_H */

