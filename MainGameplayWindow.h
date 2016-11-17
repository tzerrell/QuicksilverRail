/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MainGameplayWindow.h
 * Author: tnc02_000
 *
 * Created on November 15, 2016, 3:59 PM
 */

#ifndef MAINGAMEPLAYWINDOW_H
#define MAINGAMEPLAYWINDOW_H

#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QMainWindow>

class boardWindow;

class MainGameplayWindow : public QMainWindow {
public:
    MainGameplayWindow(QWidget* parent = Q_NULLPTR);
    MainGameplayWindow(const MainGameplayWindow&) = delete;
    virtual ~MainGameplayWindow();
    
public slots:
    void runTestAction();
protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
private:
    QAction* testAct;
    QMenuBar* mainMenuBar;
    QMenu* fileMenu;
    boardWindow* mainView;
    QWidget* centralContainer;
};

#endif /* MAINGAMEPLAYWINDOW_H */

