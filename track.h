/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   track.h
 * Author: tnc02_000
 *
 * Created on October 31, 2016, 1:42 AM
 */

#ifndef TRACK_H
#define TRACK_H

class player;

enum class track_t {
    rail,
    broken,
    sea,
    none
};

struct track {
public:    
    track();
    track(int i): data(i) {}
    track(const track& orig);
    virtual ~track();
    
    track_t type();
    player* owner();
private:
    int data;
};

#endif /* TRACK_H */

