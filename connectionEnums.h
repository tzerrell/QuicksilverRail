/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   connectionEnums.h
 * Author: tnc02_000
 *
 * Created on November 13, 2016, 12:27 AM
 */

#ifndef CONNECTIONENUMS_H
#define CONNECTIONENUMS_H



enum class track_t {
    none,
    rail,
    broken,
    sea
};

enum class crossing_t {
    land,
    river,
    strait,
    city,   //for the unowned public intra-city track 
    water   //at least one vertex is a water vertex
};


#endif /* CONNECTIONENUMS_H */

