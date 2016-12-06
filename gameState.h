/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   gameState.h
 * Author: tnc02_000
 *
 * Created on December 6, 2016, 6:39 AM
 */

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>

#include <player.h>

class gameState {
public:
    gameState();
    gameState(const gameState& orig);
    virtual ~gameState();
    
    enum class mode {
        noGame,
        gameSetup,
        activeTurn,
        betweenTurns
    };
    
    void endTurn();
    std::vector<player*> listPlayers();
    player* currentPlayer() { return currPlayer; };
private:
    std::vector<player> players;
    player* currPlayer;
    mode currMode;
    //std::vector<effects> currEffects; //TODO: once effects exist
};

#endif /* GAMESTATE_H */

