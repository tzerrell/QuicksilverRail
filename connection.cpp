/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   connection.cpp
 * Author: tnc02_000
 * 
 * Created on October 31, 2016, 7:01 PM
 */

#include "connection.h"

connection::connection()
        : crossing(crossing_t::water)
        , track(track_t::none)
        , owner(nullptr)
{
}

connection::connection(crossing_t c, track_t t, player* p)
        : crossing(c)
        , track(t)
        , owner(p)
{
    
}

connection::connection(connection&& source)
        : crossing(source.crossing)
        , track (source.track)
        , owner (source.owner)
{
    
}

connection::~connection() {
}

void connection::reset(crossing_t c, track_t t, player* p) {
    crossing = c;
    track = t;
    owner = p;
}

