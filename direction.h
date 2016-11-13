/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   direction.h
 * Author: tnc02_000
 *
 * Created on October 31, 2016, 1:33 AM
 */

#ifndef DIRECTION_H
#define DIRECTION_H

enum class direction {
    E,
    NE,
    NW,
    W,
    SW,
    SE
};

direction operator-(const direction& in);

#endif /* DIRECTION_H */

