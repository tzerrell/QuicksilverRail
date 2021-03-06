/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MainGameplayWindow.cpp
 * Author: tnc02_000
 * 
 * Created on November 15, 2016, 3:59 PM
 */

#include <QMenuBar>
#include <QStatusBar>
#include <QtGui/QResizeEvent>

#include <iostream>

#include "boardWindow.h"
#include "board.h"

//TODO: Temp?
#include "location.h"

#include "MainGameplayWindow.h"

MainGameplayWindow::MainGameplayWindow(QWidget* parent)
        : QMainWindow(parent)
{
    
    testAct = new QAction(tr("&Test"), this);
    testAct->setShortcuts(QKeySequence::New);
    testAct->setStatusTip(tr("A test action (TODO!)"));
    connect(testAct, &QAction::triggered, this, &MainGameplayWindow::runTestAction);
    
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(testAct);
    fileMenu->addSeparator();
    
    mainView = new boardWindow();
    mainView->setAnimating(true);
    centralContainer = QWidget::createWindowContainer(mainView, this);
    centralContainer->setMouseTracking(true);
    mainView->setRootWindow(this);
    setMouseTracking(true);
    //menuBar()->setMouseTracking(true);
    statusBar()->setMouseTracking(true);
    
    centralContainer->setGeometry(0, 
            menuBar()->height(),
            width(),
            height() - menuBar()->height() - statusBar()->height() );
}

MainGameplayWindow::~MainGameplayWindow() {
    //TODO: destruct the things new'd in the constructor (several)
}

void MainGameplayWindow::runTestAction() {
    std::cout << "Test Action Run!\n"; //TODO: real actions
}

void MainGameplayWindow::mouseMoveEvent(QMouseEvent *event) {
    
    //TODO: This is debug info; change to real status tips
    QString internalCoords("Internal Coords: ");
    QString strBuilder;
    strBuilder.setNum(event->x(), 10); internalCoords.append(strBuilder);
    internalCoords.append(",");
    strBuilder.setNum(event->y(), 10); internalCoords.append(strBuilder);
    internalCoords.append("; (i,j): (");
    board::coord pt(event->x(), event->y(), mainView);
    strBuilder.setNum(pt.i(), 10); internalCoords.append(strBuilder);
    internalCoords.append(",");
    strBuilder.setNum(pt.j(), 10); internalCoords.append(strBuilder);
    internalCoords.append(") (l,m): (");
    strBuilder.setNum(pt.l(), 10); internalCoords.append(strBuilder);
    internalCoords.append(",");
    strBuilder.setNum(pt.m(), 10); internalCoords.append(strBuilder);
    internalCoords.append(") (s,t): (");
    strBuilder.setNum(pt.s()); internalCoords.append(strBuilder);
    internalCoords.append(",");
    strBuilder.setNum(pt.t()); internalCoords.append(strBuilder);
    internalCoords.append(")");
    if (mainView->getSubject()->isOnBoard(pt)) {
        internalCoords.append(", on board");
        internalCoords.append(", alt-(i,j): (");
        board::coord orthoPt(pt.i(),pt.j(),mainView->getSubject());
        location* mouseLoc = mainView->getSubject()->getLocation(orthoPt);
        strBuilder.setNum(mouseLoc->getCoord().i(), 10); internalCoords.append(strBuilder);
        internalCoords.append(",");
        strBuilder.setNum(mouseLoc->getCoord().j(), 10); internalCoords.append(strBuilder);
        internalCoords.append(")");
    }
    else
        internalCoords.append(", off board");
    internalCoords.append("; connection: (");
    strBuilder.setNum(pt.connI()); internalCoords.append(strBuilder);
    internalCoords.append(",");
    strBuilder.setNum(pt.connJ()); internalCoords.append(strBuilder);
    internalCoords.append(",");
    switch (pt.connDir()) {
    case direction::E:
        internalCoords.append("E)");
        break;
    case direction::NE:
        internalCoords.append("NE)");
        break;
    case direction::NW:
        internalCoords.append("NW)");
        break;
    default:
        internalCoords.append("ERROR)");
        std::cerr << "String display error from invalid connDir()\n";
        break;
    }
    
    statusBar()->showMessage(internalCoords);
    QMainWindow::mouseMoveEvent(event);
}

bool MainGameplayWindow::event(QEvent *event)
{
    //TODO: Add events here
    switch (event->type()) {
        case QEvent::Resize:
            centralContainer->setGeometry(0, 
                    menuBar()->height(),
                    ((QResizeEvent*)event)->size().width(), 
                    ((QResizeEvent*)event)->size().height() 
                    - menuBar()->height() - statusBar()->height());
            return QMainWindow::event(event);
        default:
            return QMainWindow::event(event);
    }
}
