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

//EnumTraits keeps track of special properties of enum classes (or enums in
//general). Things like the first or last element of the enum, or the first or
//last standard element.
//
template<typename E> struct EnumTraits;

enum class terrain {
    Plains,
    Forest,
    Hills,
    Mountains,
    Swamp,
    Desert,
    Jungle,
    Town,
    Castle,
    City,
    Port,
    Rock,
    Cavern,
    Tunnels,
    Sea,
    ContinentalShelf,
    AbyssalPlain,
    Reef,
    Invalid
};

enum class crossing {
    Land,
    River,
    Channel,
    Sea,
    Invalid
};

template<> struct EnumTraits<terrain> { 
    static constexpr terrain FIRST = terrain::Plains; 
    static constexpr terrain LAST = terrain::Reef;
    static constexpr terrain LOGICAL_LAST = terrain::Invalid;   //Not a true terrain, but the last entry in the enum class
    static constexpr terrain FIRST_NATURAL_LAND = terrain::Plains; 
    static constexpr terrain LAST_NATURAL_LAND = terrain::Jungle;
    static constexpr terrain FIRST_INHABITED = terrain::Town; 
    static constexpr terrain LAST_INHABITED = terrain::Port;
    static constexpr terrain FIRST_UNDERGROUND = terrain::Rock;
    static constexpr terrain LAST_UNDERGROUND = terrain::Tunnels;
    static constexpr terrain FIRST_OCEANIC = terrain::Sea;
    static constexpr terrain LAST_OCEANIC = terrain::Reef;
};

terrain& operator++(terrain& t);

template<> struct EnumTraits<crossing> { 
    static constexpr crossing FIRST = crossing::Land; 
    static constexpr crossing LAST = crossing::Sea;
    static constexpr crossing LOGICAL_LAST = crossing::Invalid; //Not a true crossing, but the last entry in the enum class
    static constexpr crossing FIRST_BUILDABLE = crossing::Land; 
    static constexpr crossing LAST_BUILDABLE = crossing::Channel;
};

namespace Enum {
    template<typename T> std::size_t count();
    template<> std::size_t count<terrain>();
}

#endif /* TERRAIN_H */

