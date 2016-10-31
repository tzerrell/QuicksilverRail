/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

//created referencing http://doc.qt.io/qt-5/qtgui-openglwindow-example.html

/* 
 * File:   boardWindow.h
 * Author: tnc02_000
 *
 * Created on October 31, 2016, 2:18 PM
 */

#ifndef BOARDWINDOW_H
#define BOARDWINDOW_H

#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>

#include <qopengl.h>

class board;

class boardWindow : public QWindow, protected QOpenGLFunctions {
    Q_OBJECT
public:
    explicit boardWindow(QWindow* parent = 0);
    boardWindow(const boardWindow&) = delete;
    virtual ~boardWindow();
    
    void setAnimating(bool val) { 
        animating = val;
        if (animating) renderLater();
    };
public slots:
    void render();
    void renderLater();
protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    void exposeEvent(QExposeEvent *event) Q_DECL_OVERRIDE;
private:
    board* subject;
    bool animating;
    bool updatePending;
    QOpenGLContext *context;
    QRectF view;
};

#endif /* BOARDWINDOW_H */

