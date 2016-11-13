/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   connection.h
 * Author: tnc02_000
 *
 * Created on October 31, 2016, 7:01 PM
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include "connectionEnums.h"

class player;

class connection {
public:
    connection();
    connection(crossing_t c, track_t t = track_t::none, player* p = nullptr);
    connection(const connection&) = delete;
    connection(connection&& source);
    virtual ~connection();
    
    void reset(crossing_t c, track_t t, player* p);
private:
    crossing_t crossing;
    track_t track;
    player* owner;
};

#endif /* CONNECTION_H */

