/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   player.h
 * Author: tnc02_000
 *
 * Created on October 31, 2016, 10:13 AM
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <cstddef>
#include <array>

#include <QtGui/QOpenGLFunctions>

#include "train.h"
#include "delivery.h"
#include "engineer.h"

class player {
public:
    player();
    player(const player& orig);
    virtual ~player();
    
    std::array<GLfloat, 4> getColor() { return color; };
    void setColor(std::array<GLfloat, 4> col) { color = col; };
    void setColor(const GLfloat* col) { for (int i = 0; i < color.size(); ++i) color[i] = col[i]; };
    
    bool isActive() { return active; };
    void setActive(bool actv) { active = actv; };
private:
    static constexpr std::size_t num_deliveries = 3;
    train trn;
    short playerID;
    int money;
    bool active;
    engineer eng;
    std::array<delivery,num_deliveries> deliv;
    std::array<GLfloat, 4> color;   //RGBA components in range [0.0, 1.0]
};

#endif /* PLAYER_H */

