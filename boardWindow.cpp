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
#include <fstream>
#include <sstream>

#include "board.h"

#include "boardWindow.h"
#include <qopengl.h>

boardWindow::boardWindow(QWindow* parent)
        : QWindow(parent)
        , subject(nullptr)
        , animating(false)
        , context(0)
        , view(-10,-10,50,50)    //TODO: Choose an appropriate default view
        , locHorizSpacing(20.0f)
        , locVertSpacing(14.0f)
        , verbose(true)
{
    subject = new board;    //TODO: do this in a reasonable way ...
    surfaceFormat.setSamples(4);
    setSurfaceType(QWindow::OpenGLSurface);
}

boardWindow::~boardWindow() {
    delete subject;
    if (context) delete context;
}

void boardWindow::testRender() {
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
        if (verbose) std::cout << "Initializing boardWindow for testing.\n";
        initializeOpenGLFunctions();
        
        constructTestBuffers();
        //TODO constructGLBuffers();
        glClearColor(0.7f,0.7f,0.7f,1.0f);
    }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    locationVertexBuffer.bind();
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE,0,(void*)0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);
    
    context->swapBuffers(this);
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
        if (verbose) std::cout << "Initializing boardWindow.\n";
        initializeOpenGLFunctions();
        
        //other OpenGL initialization code goes here
        updateShaders("generic.vertexshader", "generic.fragmentshader");    //TODO: adjust to allow custom filenames
        constructGLBuffers();
        glClearColor(0.7f,0.7f,0.7f,1.0f);
    }
    
    const qreal pixelRatio = devicePixelRatio();
    glViewport(0, 0, width() * pixelRatio, height() * pixelRatio);
    
    QMatrix4x4 renderMatrix;
    renderMatrix.ortho(view);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //TODO: render scene
    //TODO: do this in a real way
    locationVertexBuffer.bind();
    locationStripElementBuffer[0].bind();
    
    int TODOoffset = 0;
    
    int vertPositionHandle = shaderProgram.attributeLocation("TODOposition"); //TODO
    shaderProgram.enableAttributeArray(vertPositionHandle);
    shaderProgram.setAttributeBuffer(vertPositionHandle, GL_FLOAT, TODOoffset, 
            3, sizeof(GLfloat /*TODO*/));
    
    locationVertexBuffer.bind();
    locationStripElementBuffer[0].bind();
    
    //TODO
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,6,GL_FLOAT, GL_FALSE,0,(void*)0);
    
    //glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_SHORT, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 6 /*TODO*/);
    //TODO: End of test render
    
    //TODO
    glDisableVertexAttribArray(0);
    
    context->swapBuffers(this);
    
    if (animating) renderLater();
}

bool boardWindow::event(QEvent *event)
{
    //TODO: Add events here
    switch (event->type()) {
    case QEvent::UpdateRequest:
        updatePending = false;
        //TODO testRender();
        render();
        return true;
    default:
        return QWindow::event(event);
    }
}

void boardWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);
    if (isExposed()) testRender();
}

void boardWindow::renderLater() {
    if (!updatePending) {
        updatePending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

bool boardWindow::updateShaders(std::string vertShaderFile, std::string fragShaderFile) {
    vertexShaderFilename = vertShaderFile;
    fragmentShaderFilename = fragShaderFile;
    return createShaderProgram();
}

/*
std::string boardWindow::readFileToString(std::string filename) {
    std::ostringstream codeStream;
    std::ifstream fin;
    
    if (verbose) {
        std::cout << "Loading file \"" << filename << "\" to string.\n";
    }
    
    fin.open(filename);
    if(fin.is_open()) {
        codeStream << fin.rdbuf();
        fin.close();
        return codeStream.str();
    }
    else {
        std::cerr << "Unable to open file " << filename << "\n";
        fin.close();
        return std::string("");
    }
}
*/

bool boardWindow::createShaderProgram() {
    QOpenGLShader vShader(QOpenGLShader::Vertex);
    QOpenGLShader fShader(QOpenGLShader::Fragment);
    if (!vShader.compileSourceFile(QString::fromStdString(vertexShaderFilename))) {
        std::cerr << "Unable to compile vertex shader from \""
                << vertexShaderFilename << "\"\n";
        std::string utf8_log = fShader.log().toUtf8().constData();
        std::cerr << "GLSL compiler errors (expect " << utf8_log.size()
                << " characters):" << utf8_log << "\n";
        return false;
    }
    if (!fShader.compileSourceFile(QString::fromStdString(fragmentShaderFilename))) {
        std::cerr << "Unable to compile fragment shader from \""
                << fragmentShaderFilename << "\"\n";
        std::string utf8_log = fShader.log().toUtf8().constData();
        std::cerr << "GLSL compiler errors:" << utf8_log << "\n";
        return false;
    }
    
    if (!shaderProgram.addShader(&vShader)) {
        std::cerr << "Unable to add vertex shader to shader program.\n";
        std::string utf8_log = shaderProgram.log().toUtf8().constData();
        std::cerr << "OpenGL errors:" << utf8_log << "\n";
        return false;
    }
    if (!shaderProgram.addShader(&fShader)) {
        std::cerr << "Unable to add fragment shader to shader program.\n";
        std::string utf8_log = shaderProgram.log().toUtf8().constData();
        std::cerr << "OpenGL errors:" << utf8_log << "\n";
        return false;
    }
    if (!shaderProgram.link()) {
        std::cerr << "Unable to link shader program.\n";
        std::string utf8_log = shaderProgram.log().toUtf8().constData();
        std::cerr << "GLSL Linker errors:" << utf8_log << "\n";
        return false;
    }
    if (!shaderProgram.bind()) {
        std::cerr << "Unable to bind shader program to context.\n";
        return false;
    }
    return true;
}

bool boardWindow::constructTestBuffers() {
    std::vector<GLfloat> vertexCoords;
    if (verbose) std::cout << "Constructing test buffers.\n";
    vertexCoords.push_back(-0.5); vertexCoords.push_back(-0.5);
    vertexCoords.push_back( 0.5); vertexCoords.push_back(-0.5);
    vertexCoords.push_back( 0.0); vertexCoords.push_back( 1.0);
    
    locationVertexBuffer.create();
    locationVertexBuffer.bind();
    locationVertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    locationVertexBuffer.allocate(&vertexCoords[0], 6*sizeof(vertexCoords[0]));
    
    return true;
}

bool boardWindow::constructGLBuffers() {
    //construct a vertex buffer for the location icon quadrilaterals
    std::vector<GLfloat> vertexCoords;
    if (verbose) std::cout << "Constructing locationVertexBuffer.\n";
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
                << " is too large. Location vertex buffer not constructed.\n";
        return false;
    }
    locationVertexBuffer.create();
    locationVertexBuffer.bind();
    locationVertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    locationVertexBuffer.allocate(&vertexCoords[0], vertexCoords.size()*sizeof(vertexCoords[0]));
    locationVertexBuffer.release();
    
    //construct an element buffer for each strip of quads
    std::vector<GLushort> vertexIndices;
    std::vector<GLushort> stripStartIndices;
    GLushort currIndex = 0;
    try {
        for (int i = 0; i < subject->getNumRows(); ++i) {
            int rowParity = i%2;
            for (int j = 0; j < subject->getNumCols() + 1 + rowParity; ++j) {
                vertexIndices.push_back(currIndex);
                if (j == 0) {
                    stripStartIndices.push_back(currIndex);
                }
                ++currIndex;
            }
        }
        stripStartIndices.push_back(currIndex);
    }
    catch (const std::bad_alloc& ex) {
        std::cerr << "Bad allocation in constructGLBuffers. The board"
                << " is too large. Location element buffers not constructed.\n";
        std::cerr << "Warning: Partial construction of GLBuffers.\n";
        return false;
    }
    
    for (int i = 0; i < subject->getNumRows(); ++i) {
        QOpenGLBuffer currBuffer;
        currBuffer.create();
        currBuffer.bind();
        currBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        currBuffer.allocate(&vertexIndices[stripStartIndices[i]] 
                , sizeof(vertexIndices[0])
                * (stripStartIndices[i+1] - stripStartIndices[i]));
        currBuffer.release();
        locationStripElementBuffer.push_back(currBuffer);
    }
    
    //TODO: Same thing for connectionVertexBuffer
    
    return true;
}