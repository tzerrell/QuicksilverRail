/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdexcept>

#include "direction.h"

direction operator-(const direction& in) {
    switch (in) {
        case (direction::E):
            return direction::W;
        case (direction::W):
            return direction::E;
        case (direction::NE):
            return direction::SW;
        case (direction::SW):
            return direction::NE;
        case (direction::NW):
            return direction::SE;
        case (direction::SE):
            return direction::NW;
    }
    throw (std::out_of_range("Invalid direction passed to unary minus."));
}

direction operator++(direction& dir) {
    if (dir == direction::SE)
        dir = direction::E;
    else
        dir = static_cast<direction>(static_cast<int>(dir) + 1);
    return dir;
}

direction operator--(direction& dir) {
    if (dir == direction::E)
        dir = direction::SE;
    else
        dir = static_cast<direction>(static_cast<int>(dir) - 1);
    return dir;
}
