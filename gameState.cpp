/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   gameState.cpp
 * Author: tnc02_000
 * 
 * Created on December 6, 2016, 6:39 AM
 */

#include "gameState.h"

gameState::gameState() {
    //TODO: For now, we default construct a single player game. Probably not
    //what we want for the long term.
    
    players.push_back(player());
    players[0].setColor(std::array<GLfloat,4> {0,1,0,1}); 
    currPlayer = &players[0];
}

gameState::gameState(const gameState& orig) {
}

gameState::~gameState() {
}

