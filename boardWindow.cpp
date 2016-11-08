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
#include <QtGui/QOpenGLTexture>
#include <QtDebug>

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
        //, view(-20,-30,96,72)    //TODO: Choose an appropriate default view
        , view(-40, -60, 192, 144)
        , locHorizSpacing(20.0f)
        , locVertSpacing(14.0f)
        , verbose(true)
        , locationIndexBuffer(QOpenGLBuffer::IndexBuffer)
{
    subject = new board;    //TODO: do this in a reasonable way ...
    surfaceFormat.setSamples(4);
    setSurfaceType(QWindow::OpenGLSurface);
}

boardWindow::~boardWindow() {
    delete subject;
    if (context) delete context;
    if (debugLogger) delete debugLogger;
}

void boardWindow::initGL() {
    updateShaders("generic.vertexshader", "generic.fragmentshader");    //TODO: adjust to allow custom filenames
    projMatrixHandle = glGetUniformLocation(shaderProgram.programId(), "projectionMat");
    constructGLBuffers();
    //TODO: Once I have objects at different heights, enable depth testing
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    glClearColor(0.7f,0.7f,0.7f,1.0f);
}

void boardWindow::printDebugLog() {
    //TODO: My system appears to not support QOpenGLDebugMessages
    qDebug() << "Test TODO Qt Debug Message\n";
    const QList<QOpenGLDebugMessage> messages = debugLogger->loggedMessages();
    for (const QOpenGLDebugMessage &message : messages)
        qDebug() << message;
}

void boardWindow::render() {
    if (!isExposed()) return;
    
    bool uninitialized = false;
    if (!context) {
        context = new QOpenGLContext(this); //TODO: is deleting in dtor right?
        debugLogger = new QOpenGLDebugLogger(); //TODO: is deleting in dtor right?
        if (!debugLogger->initialize())
            std::cerr << "Unable to initialize OpenGL Debug Logger.\n";
        QSurfaceFormat format(QSurfaceFormat::defaultFormat());
        format.setAlphaBufferSize(8);
        format.setOption(QSurfaceFormat::DebugContext);
        context->setFormat(format);
        context->create();
        debugLogger->startLogging();
        
        //We would use this if we had VAOs, but we're using OpenGL 2 so we don't
        //glGenVertexArrays(1, &vertexArrayID);
        //glBindVertexArray(vertexArrayID);
        
        uninitialized = true;
    }
    context->makeCurrent(this);
    if (uninitialized) {
        if (verbose) std::cout << "Initializing boardWindow.\n";
        initializeOpenGLFunctions();
        initGL();   //other OpenGL initialization code goes here
    }
    
    const qreal pixelRatio = devicePixelRatio();
    glViewport(0, 0, width() * pixelRatio, height() * pixelRatio);
    
    //Pass our orthographic projection matrix to GLSL
    QMatrix4x4 renderMatrix;
    renderMatrix.ortho(view);
    glUniformMatrix4fv(projMatrixHandle, 1, GL_FALSE, renderMatrix.constData());
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //TODO: render scene
    //TODO: do this in a real way
    locationVertexBuffer.bind();
    
    int vertPositionHandle = shaderProgram.attributeLocation("position");
    shaderProgram.enableAttributeArray(vertPositionHandle);
    shaderProgram.setAttributeBuffer(vertPositionHandle, GL_FLOAT, 0, 
            3, sizeof(GLfloat /*TODO*/));
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE,0,(void*)0);
    
    /*
    QImage texTODO2;
    if (!texTODO2.load(":/terrMountains.png")) {
        std::cerr << "Unable to load image ':/terrMountains.png'\n";
    }
    texTODO2 = texTODO2.mirrored();
    GLuint texTODO2ID;
    glGenTextures(1, &texTODO2ID);
    glBindTexture(GL_TEXTURE_2D, texTODO2ID);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, texTODO2.width(), texTODO2.height()
            , 0, GL_RGB, GL_UNSIGNED_BYTE, texTODO2.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,4,GL_UNSIGNED_BYTE, GL_FALSE, 0, (void*)0);
    */
    
    
    QOpenGLTexture texTODOTemp(QImage(":/terrMountains.png").mirrored());
    texTODOTemp.setMinificationFilter(QOpenGLTexture::Linear);
    texTODOTemp.setMagnificationFilter(QOpenGLTexture::Linear);
    texTODOTemp.bind(2);
    shaderProgram.setUniformValue("TODOTestSampler", 2);
    
    int texUVHandle = shaderProgram.attributeLocation("UV");
    shaderProgram.enableAttributeArray(texUVHandle);
    shaderProgram.setAttributeBuffer(texUVHandle, GL_FLOAT, 0, 2,
            sizeof(GLfloat));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    int numQuads = subject->getNumRows() * subject->getNumCols() + subject->getNumRows()/2;
    locationIndexBuffer.bind();
    glDrawElements(GL_TRIANGLES, numQuads * 6, GL_UNSIGNED_SHORT, 0);
    locationIndexBuffer.release();
    texTODOTemp.release();
    locationVertexBuffer.release();
    //TODO: End of test render
    
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    
    printDebugLog();
    
    context->swapBuffers(this);
    
    if (animating) renderLater();
}

bool boardWindow::event(QEvent *event)
{
    //TODO: Add events here
    switch (event->type()) {
    case QEvent::UpdateRequest:
        updatePending = false;
        render();   //TODO: Can switch render function here for testing
        return true;
    default:
        return QWindow::event(event);
    }
}

void boardWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);
    if (isExposed()) render(); //TODO: Can switch render function here for testing
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

bool boardWindow::createShaderProgram() {
    QOpenGLShader vShader(QOpenGLShader::Vertex);
    QOpenGLShader fShader(QOpenGLShader::Fragment);
    if (!vShader.compileSourceFile(QString::fromStdString(vertexShaderFilename))) {
        std::cerr << "Unable to compile vertex shader from \""
                << vertexShaderFilename << "\"\n";
        std::string utf8_log = vShader.log().toUtf8().constData();
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

bool boardWindow::constructGLBuffers() {
    //construct a vertex buffer for the location icon quadrilaterals
    //also construct UV coordinates for textures for each vertex
    std::vector<GLfloat> vertexCoords;
    std::vector<GLfloat> vertexUVs;
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
            for (int j = 0; j < subject->getNumCols() + rowParity; ++j) {
                //The formulas for these coordinates have two parts: the part
                //with the i or j puts the center in the right place; the other
                //term moves to where the vertex should be relative to the
                //center. Note that rowParity is because every other row is
                //shifted 50% in order to make a hex grid.

                //upper left coord
                GLfloat ULx = (j - (rowParity)/2.0) * locHorizSpacing - locHorizSpacing/2.0;
                GLfloat ULy = (i - 1.0) * locVertSpacing + locHorizSpacing/2.0;
                GLfloat ULz = 0.0;
                vertexCoords.push_back(ULx);
                vertexCoords.push_back(ULy);
                vertexCoords.push_back(ULz);
                //TODO: confirm correct UV coords
                vertexUVs.push_back(0.0);   vertexUVs.push_back(1.0);
                
                //lower left coord
                GLfloat LLx = (j - (rowParity)/2.0) * locHorizSpacing - locHorizSpacing/2.0;
                GLfloat LLy = (i - 1.0) * locVertSpacing - locHorizSpacing/2.0;
                GLfloat LLz = 0.0;
                vertexCoords.push_back(LLx);
                vertexCoords.push_back(LLy);
                vertexCoords.push_back(LLz);
                vertexUVs.push_back(0.0);   vertexUVs.push_back(0.0);

                //upper right coord
                GLfloat URx = (j - (rowParity)/2.0) * locHorizSpacing + locHorizSpacing/2.0;
                GLfloat URy = (i - 1.0) * locVertSpacing + locHorizSpacing/2.0;
                GLfloat URz = 0.0;
                vertexCoords.push_back(URx);
                vertexCoords.push_back(URy);
                vertexCoords.push_back(URz);
                vertexUVs.push_back(1.0);   vertexUVs.push_back(1.0);
                
                //lower right coord
                GLfloat LRx = (j - (rowParity)/2.0) * locHorizSpacing + locHorizSpacing/2.0;
                GLfloat LRy = (i - 1.0) * locVertSpacing - locHorizSpacing/2.0;
                GLfloat LRz = 0.0;
                vertexCoords.push_back(LRx);
                vertexCoords.push_back(LRy);
                vertexCoords.push_back(LRz);
                vertexUVs.push_back(1.0);   vertexUVs.push_back(0.0);
                
                if (verbose) {
                    std::cout << "Adding vertices (" << LLx << ", " << LLy
                            << ", " << LLz << "), (" << ULx << ", " << ULy
                            << ", " << ULz << "), (" << LRx << ", " << LRy
                            << ", " << LRz << "), (" << URx << ", " << URy
                            << ", " << URz << ")\n";
                }
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
    
    //construct an element buffer for all the quads
    std::vector<GLushort> vertexIndices;
    GLushort currIndex = 0;
    try {
        if (verbose) std::cout << "Writing vertex indices: ";
        for (int i = 0; i < subject->getNumRows(); ++i) {
            int rowParity = i%2;
            if (verbose) std::cout << "i = " << i;
            for (int j = 0; j < subject->getNumCols() + rowParity; ++j) {
                if (verbose) std::cout << "\nj = " << j << " of " << subject->getNumCols() + rowParity << ":";
                for (int twice = 0; twice < 2; ++twice) {
                    if (verbose) {
                        std::cout << currIndex << ' ' << currIndex + 1 << ' '
                                << currIndex + 2 << ' ';
                    }
                    vertexIndices.push_back(currIndex);
                    vertexIndices.push_back(currIndex + 1);
                    vertexIndices.push_back(currIndex + 2);
                    ++currIndex;
                }
                ++currIndex;
                ++currIndex;
            }
            if (verbose) std::cout << '\n';
        }
    }
    catch (const std::bad_alloc& ex) {
        std::cerr << "Bad allocation in constructGLBuffers. The board"
                << " is too large. Location element buffers not constructed.\n";
        std::cerr << "Warning: Partial construction of GLBuffers.\n";
        return false;
    }
     
    //Note: locationIndexBuffer was constructed as an index buffer
    locationIndexBuffer.create();
    locationIndexBuffer.bind();
    locationIndexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    int numQuads = subject->getNumRows() * subject->getNumCols() + subject->getNumRows()/2;
    locationIndexBuffer.allocate(&vertexIndices[0] 
            , sizeof(vertexIndices[0]) * 6 * numQuads);
    locationIndexBuffer.release();
        
    if(verbose) {
        std::cout << "locationIndexBuffer contains "
                << 6*numQuads
                << " indices: ";
        for (int j = 0; j < 6*numQuads; ++j) {
            std::cout << vertexIndices[j] << " ";
        }
        std::cout << '\n';
        std::cout.flush();
    }
    
    //TODO: Same thing for connectionVertexBuffer
    
    return true;
}
