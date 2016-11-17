/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: tnc02_000
 *
 * Created on October 30, 2016, 10:15 PM
 */

#include <QApplication>
#include <QtGui/QGuiApplication>

#include <cstdio>
#include <iostream>
#include "boardWindow.h"

#include "board.h"
#include "terrain.h"
#include "player.h"
#include "textureManager.h"

#include "MainGameplayWindow.h"

void initializeLogging() {
    freopen("log/error.log","w",stderr);
    freopen("log/info.log","w",stdout);
    std::cout << "Logging to files enabled.\n";
    std::cout.flush();
}

int main(int argc, char *argv[]) {
    initializeLogging();
    
    QApplication app(argc, argv);
    
    // create and show your widgets here
    MainGameplayWindow mainWin;   //MainGameplayWindow mainWin;
    mainWin.resize(640,400);
    mainWin.show();
    /* TODO: Removed to test main window style
    boardWindow tempWindow;
    tempWindow.resize(640,400);
    tempWindow.show();
    
    tempWindow.setAnimating(true);
    */
    
    return app.exec();
}