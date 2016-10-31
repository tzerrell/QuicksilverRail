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

#include "board.h"
#include "terrain.h"
#include "player.h"
#include "track.h"

int main(int argc, char *argv[]) {
    // initialize resources, if needed
    // Q_INIT_RESOURCE(resfile);

    QApplication app(argc, argv);

    // create and show your widgets here

    return app.exec();
}
