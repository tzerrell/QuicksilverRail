/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   train.h
 * Author: tnc02_000
 *
 * Created on October 31, 2016, 10:44 AM
 */

#ifndef TRAIN_H
#define TRAIN_H

#include <vector>
#include <string>
#include "goods.h"

class player;
class board;

class train {
public:
    train();
    train(const train& orig);
    virtual ~train();
private:
    struct {
        int maxMove;
        int maxLoad;
        int level;
        std::string name;
    } type;
    int curMove;
    std::vector<good> load;
    struct {
        int x; int y;
    } pos;
    
    player* owner;
    board* world;
};

#endif /* TRAIN_H */

