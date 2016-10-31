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

class player {
public:
    player();
    player(const player& orig);
    virtual ~player();
private:
    static constexpr std::size_t num_deliveries = 3;

};

#endif /* PLAYER_H */

