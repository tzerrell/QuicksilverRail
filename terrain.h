/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   terrain.h
 * Author: tnc02_000
 *
 * Created on October 31, 2016, 1:21 AM
 */

#ifndef TERRAIN_H
#define TERRAIN_H

enum class terrain {
    Plains,
    Forest,
    Hills,
    Mountains,
    Swamp,
    Desert,
    Jungle,
    Town,
    City,
    Port
};

enum class crossing {
    Land,
    River,
    Channel,
    Sea
};

#endif /* TERRAIN_H */

