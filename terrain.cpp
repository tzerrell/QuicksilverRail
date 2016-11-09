/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <cstddef>

#include "terrain.h"

terrain& operator++(terrain& t) {
    using BaseType = typename std::size_t;
    if (t == EnumTraits<terrain>::LOGICAL_LAST) {
        t = static_cast<terrain>(0);
        return t;
    }
    t = static_cast<terrain>( static_cast<BaseType>(t) + 1 );
    return t;
}
