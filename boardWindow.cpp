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

#include <iostream>

#include "board.h"

#include "boardWindow.h"
#include <qopengl.h>

boardWindow::boardWindow(QWindow* parent)
        : QWindow(parent)
        , subject(nullptr)
        , animating(false)
        , context(0)
        , view(10,10,50,50)    //TODO: Choose an appropriate default view
        , locHorizSpacing(20.0f)
        , locVertSpacing(14.0f)
{
    surfaceFormat.setSamples(4);
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
        
        //We would use this if we had VAOs, but we're using OpenGL 2 so we don't
        //glGenVertexArrays(1, &vertexArrayID);
        //glBindVertexArray(vertexArrayID);
        
        uninitialized = true;
    }
    context->makeCurrent(this);
    if (uninitialized) {
        initializeOpenGLFunctions();
        
        //other OpenGL initialization code goes here
        glClearColor(1.0f,1.0f,1.0f,1.0f);
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

bool boardWindow::constructVertexBuffers() {
    std::vector<GLfloat> vertexCoords;
    try {
        /* For each row, produce line of triangles like so:
         * 2--4--6--8
         * |\ |\ |\ |
         * | \| \| \|
         * 1--3--5--7
         * with vertices produced in the indicated order. Each row will have
         * numCols + rowParity rectangles, each of which has two vertices plus
         * one for the last pair. Each vertex has 3 coordinates. The centers of
         * the rects are at 
         *   ( (col - parity/2.0f)*locHorizSpacing , (row-1)*locVertSpacing)
         * and the rectangles extend up and down and left and right a distance
         * of vertexIconWidth/2.0f from these centers.
         * 
         * Note that for these to connect correctly, we must have 
         *               vertexIconWidth == locHorizSpacing
         * and so vertexIconWidth is not a separate parameter.
         */
        for (int i = 0; i < subject->getNumRows(); ++i) {
            int rowParity = i%2;
            for (int j = 0; j < subject->getNumCols() + 1 + rowParity; ++j) {
                //lower left coord
                vertexCoords.push_back((j - (rowParity)/2.0) * locHorizSpacing
                        - locHorizSpacing/2.0);
                vertexCoords.push_back((i - 1.0) * locVertSpacing
                        - locHorizSpacing/2.0);
                vertexCoords.push_back(0.0);

                //upper left coord
                vertexCoords.push_back((j - (rowParity)/2.0) * locHorizSpacing
                        - locHorizSpacing/2.0);
                vertexCoords.push_back((i - 1.0) * locVertSpacing
                        - locHorizSpacing/2.0);
                vertexCoords.push_back(0.0);
            }
        }
    }
    catch (const std::bad_alloc& ex) {
            std::cerr << "Bad allocation in constructVertexBuffers. The board"
                    << " is too large. Vertex buffers not constructed.\n";
            return false;
    }
    
    glDeleteBuffers(1, &locationVertexBufferID);    //if there are any previous contents, delete them
    glGenBuffers(1, &locationVertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, locationVertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertexCoords.size() * sizeof(GLfloat),
            &vertexCoords[0], GL_STATIC_DRAW);
    
    //TODO: Same thing for connectionVertexBuffer
    
    return true;
}