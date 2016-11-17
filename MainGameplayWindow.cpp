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
#include <QtGui/QResizeEvent>

#include "boardWindow.h"

#include "MainGameplayWindow.h"

MainGameplayWindow::MainGameplayWindow(QWidget* parent)
        : QMainWindow(parent)
{
    
    testAct = new QAction(tr("&Test"), this);
    testAct->setShortcuts(QKeySequence::New);
    testAct->setStatusTip(tr("A test action (TODO!)"));
    connect(testAct, &QAction::triggered, this, &MainGameplayWindow::runTestAction);
    
    /*mainMenuBar = new QMenuBar(this);
    fileMenu = mainMenuBar->addMenu(tr("&File"));
    fileMenu->addAction(testAct);
    fileMenu->addSeparator();*/
    
    mainView = new boardWindow();
    centralContainer = QWidget::createWindowContainer(mainView, this);
    
    mainView->setAnimating(true);
    
}

MainGameplayWindow::~MainGameplayWindow() {
    //TODO: destruct the things new'd in the constructor (several)
}

void MainGameplayWindow::runTestAction() {
    ; //TODO
}

bool MainGameplayWindow::event(QEvent *event)
{
    //TODO: Add events here
    switch (event->type()) {
        case QEvent::Resize:
            centralContainer->resize(((QResizeEvent*)event)->size().width(), 
                    ((QResizeEvent*)event)->size().height());
            return QMainWindow::event(event);
        default:
            return QMainWindow::event(event);
    }
}
