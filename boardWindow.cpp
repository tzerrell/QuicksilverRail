/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

//created referencing http://doc.qt.io/qt-5/qtgui-openglwindow-example.html

/* 
 * File:   boardWindow.cpp
 * Author: tnc02_000
 * 
 * Created on October 31, 2016, 2:18 PM
 */

#include <QtCore/QCoreApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QMatrix4x4>

#include "boardWindow.h"
#include <qopengl.h>

boardWindow::boardWindow(QWindow* parent)
        : QWindow(parent)
        , animating(false)
        , context(0)
        , view(10,10,50,50)    //TODO: Choose an appropriate default view
{
    setSurfaceType(QWindow::OpenGLSurface);
}

boardWindow::~boardWindow() {
    if (context) delete context;
}

void boardWindow::render() {
    if (!isExposed()) return;
    
    bool uninitialized = false;
    if (!context) {
        context = new QOpenGLContext(this); //TODO: is deleting in dtor right?
        context->setFormat(requestedFormat());
        context->create();
        
        uninitialized = true;
    }
    context->makeCurrent(this);
    if (uninitialized) {
        initializeOpenGLFunctions();
        //other OpenGL initialization code goes here
    }
    
    const qreal pixelRatio = devicePixelRatio();
    glViewport(0, 0, width() * pixelRatio, height() * pixelRatio);
    
    QMatrix4x4 renderMatrix;
    renderMatrix.ortho(view);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //TODO: render scene
    
    
    context->swapBuffers(this);
    
    if (animating) renderLater();
}

bool boardWindow::event(QEvent *event)
{
    //TODO: Add events here
    switch (event->type()) {
    case QEvent::UpdateRequest:
        updatePending = false;
        render();
        return true;
    default:
        return QWindow::event(event);
    }
}

void boardWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);
    if (isExposed()) render();
}

void boardWindow::renderLater() {
    if (!updatePending) {
        updatePending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}
