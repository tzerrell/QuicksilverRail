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

#include "boardWindow.h"

#include "board.h"
#include "terrain.h"
#include "player.h"
#include "track.h"

int main(int argc, char *argv[]) {
    // initialize resources, if needed
    // Q_INIT_RESOURCE(resfile);

    QGuiApplication app(argc, argv);
    
    boardWindow tempWindow;
    tempWindow.resize(640,400);
    tempWindow.show();
    
    tempWindow.setAnimating(true);

    // create and show your widgets here

    return app.exec();
}
