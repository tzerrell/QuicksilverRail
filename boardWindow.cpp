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
#include <QtGui/QOpenGLPixelTransferOptions>
#include <QtDebug>
#include <QtGui/QResizeEvent>
#include <QtGui/QWheelEvent>

#include <iostream>
#include <fstream>
#include <sstream>

#include "terrain.h"
#include "board.h"

#include "boardWindow.h"
#include "location.h"
#include <qopengl.h>

boardWindow::boardWindow(QWindow* parent)
        : QWindow(parent)
        , subject(nullptr)
        , animating(false)
        , context(0)
        , view(-20,-30,96,72)    //TODO: Choose an appropriate default view
        , zoomLevel(0)
        , zoomFactor(0.5)
        , locHorizSpacing(20.0f)
        , locVertSpacing(14.0f)
        , connSlashOverwidth(locHorizSpacing * 0.16f)
        , connSlashOverheight(locVertSpacing * 0.33f)
        , connDashOverwidth(locHorizSpacing * 0.33f)
        , connDashHeight(locHorizSpacing)
        , verbose(true)
        , terrainTextureAtlas(QOpenGLTexture::Target2DArray)
        , connectionTextureAtlas(QOpenGLTexture::Target2DArray)
        , locationIndexBuffer(QOpenGLBuffer::IndexBuffer)
        , connectionIndexBuffer(QOpenGLBuffer::IndexBuffer)
{
    subject = new board;    //TODO: do this in a reasonable way ...
    surfaceFormat.setSamples(4);
    setSurfaceType(QWindow::OpenGLSurface);
    
    //TODO: Make this less ad hoc
    std::string texFilenames("");
    std::stringstream fnamestr(texFilenames);
    fnamestr << ":/terrPlains.png\n:/terrMountains.png\n:/terrSwamp.png";   //TODO
    loadTerrainTextureFilenames(fnamestr);
}

boardWindow::~boardWindow() {
    delete subject;
    if (context) delete context;
    if (debugLogger) delete debugLogger;
}

void boardWindow::setRootWindow(QMainWindow* newRoot) {rootWin = newRoot;}

void boardWindow::initGL() {
    //TODO: debug code
    subject->setLocationTerrain(1,1, static_cast<terrain>(1));
    subject->setLocationTerrain(2,3, static_cast<terrain>(2));
    //TODO: end debug
    
    
    updateShaders("generic.vertexshader", "generic.fragmentshader", &shaderProgram);    //TODO: adjust to allow custom filenames
    updateShaders("generic.vertexshader", "fixedColor.fragmentshader", &fixedColorShaderProgram);
    projMatrixHandle = glGetUniformLocation(shaderProgram.programId(), "projectionMat");
    constructGLBuffers();
    loadTerrainTextures();
    //TODO: Once I have objects at different heights, enable depth testing
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    glClearColor(0.7f,0.7f,0.7f,1.0f);
}

void boardWindow::printDebugLog() {
    //Note: My system appears to not support QOpenGLDebugMessages
    qDebug() << "Test Qt Debug Message\n";
    const QList<QOpenGLDebugMessage> messages = debugLogger->loggedMessages();
    for (const QOpenGLDebugMessage &message : messages)
        qDebug() << message;
    
    //TODO: Make this either above -or- below, not both.
    
    GLenum errorCode = GL_NO_ERROR;
    do {
        errorCode = glGetError();
        if (errorCode != GL_NO_ERROR) {
            std::cerr << "OpenGL Error: " << errorCode << "\n";
        }
    } while (errorCode != GL_NO_ERROR);
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //Render scene
    //Locations
    locationVertexBuffer.bind();
    int vertPositionHandle = shaderProgram.attributeLocation("position");
    shaderProgram.enableAttributeArray(vertPositionHandle);
    shaderProgram.setAttributeBuffer(vertPositionHandle, GL_FLOAT, 0, 
            3, sizeof(GLfloat));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE,0,(void*)0);
    
    locationUVBuffer.bind();
    int texUVHandle = shaderProgram.attributeLocation("UV");
    shaderProgram.enableAttributeArray(texUVHandle);
    shaderProgram.setAttributeBuffer(texUVHandle, GL_FLOAT, 0, 2,
            sizeof(GLfloat));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    locationTerrainTypeBuffer.bind();
    int texTerrTypeHandle = shaderProgram.attributeLocation("terrain");
    shaderProgram.enableAttributeArray(texTerrTypeHandle);
    shaderProgram.setAttributeBuffer(texTerrTypeHandle, GL_FLOAT, 0, 1,
            sizeof(GLfloat));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,1,GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    terrainTextureAtlas.bind(3);
    if (!terrainTextureAtlas.isBound()) {
        std::cerr << "Error binding terrain texture atlas.\n";
    }
    int terrainTextureHandle[1] = { 3 };
    shaderProgram.setUniformValueArray("terrainTextures", terrainTextureHandle, 1);
    
    int numQuads = subject->getNumRows() * subject->getNumCols() + subject->getNumRows()/2;
    locationIndexBuffer.bind();
    glDrawElements(GL_TRIANGLES, numQuads * 6, GL_UNSIGNED_SHORT, 0);
    locationIndexBuffer.release();
    terrainTextureAtlas.release();
    locationVertexBuffer.release();
    
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    
    //Connections
    connectionVertexBuffer.bind();
    vertPositionHandle = fixedColorShaderProgram.attributeLocation("position");
    fixedColorShaderProgram.enableAttributeArray(vertPositionHandle);
    fixedColorShaderProgram.setAttributeBuffer(vertPositionHandle, GL_FLOAT, 0, 
            3, sizeof(GLfloat));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE,0,(void*)0);
    
    connectionUVBuffer.bind();
    texUVHandle = fixedColorShaderProgram.attributeLocation("UV");
    fixedColorShaderProgram.enableAttributeArray(texUVHandle);
    fixedColorShaderProgram.setAttributeBuffer(texUVHandle, GL_FLOAT, 0, 2,
            sizeof(GLfloat));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    connectionDashSlashBuffer.bind();
    texTerrTypeHandle = fixedColorShaderProgram.attributeLocation("terrain");
    fixedColorShaderProgram.enableAttributeArray(texTerrTypeHandle);
    fixedColorShaderProgram.setAttributeBuffer(texTerrTypeHandle, GL_FLOAT, 0, 1,
            sizeof(GLfloat));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,1,GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    //TODO: Need textures here
    //TODO: Using fake textures
    connectionTextureAtlas.bind(3);
    if (!connectionTextureAtlas.isBound()) {
        std::cerr << "Error binding terrain texture atlas.\n";
    }
    terrainTextureHandle[0] = 3;
    shaderProgram.setUniformValueArray("terrainTextures", terrainTextureHandle, 1);
    //TODO: End of fake textures
    
    int numVerts = subject->getNumRows() * subject->getNumCols() + subject->getNumRows()/2;
    numQuads = numVerts * 3 - 2 * subject->getNumCols() - subject->getNumRows()
            - 2 * (subject->getNumRows()/2);
    connectionIndexBuffer.bind();
    glDrawElements(GL_TRIANGLES, numQuads * 6, 
            GL_UNSIGNED_SHORT, 0);
    connectionIndexBuffer.release();
    connectionTextureAtlas.release();
    connectionVertexBuffer.release();
    
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    
    glDisable(GL_BLEND);
    
    printDebugLog();
    
    context->swapBuffers(this);
    
    if (animating) renderLater();
}

void boardWindow::mouseMoveEvent(QMouseEvent* event) {
    QCoreApplication::sendEvent(rootWin, event);
    
    //QWindow::mouseMoveEvent(event);
}

bool boardWindow::event(QEvent *event)
{
    //TODO: Add events here
    switch (event->type()) {
        case QEvent::Wheel:
            zoomLevel += ((QWheelEvent*)event)->angleDelta().y();
            zoomFactor = zoomFactorFromLevel(zoomLevel);
            view.setWidth(zoomFactor * this->width());
            view.setHeight(zoomFactor * this->height());
            return true;
        case QEvent::UpdateRequest:
            updatePending = false;
            render();   //TODO: Can switch render function here for testing
            return true;
        case QEvent::Resize:
            view.setWidth(zoomFactor * ((QResizeEvent*)event)->size().width());
            view.setHeight(zoomFactor * ((QResizeEvent*)event)->size().height());
            render();   //TODO: I thought this would fix flickering on resize, but it doesn't. Look for other solutions.
            return QWindow::event(event);
        default:
            return QWindow::event(event);
    }
}

double boardWindow::zoomFactorFromLevel(int level) {
    if (level < 0) 
        return (200 - level)/200.0;
    else
        return 200.0/(200 + level);
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

bool boardWindow::updateShaders(std::string vertShaderFile,
        std::string fragShaderFile,
        QOpenGLShaderProgram* prog) {
    vertexShaderFilename = vertShaderFile;
    fragmentShaderFilename = fragShaderFile;
    return createShaderProgram(prog);
}

bool boardWindow::createShaderProgram(QOpenGLShaderProgram* prog) {
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
    
    if (!prog->addShader(&vShader)) {
        std::cerr << "Unable to add vertex shader to shader program.\n";
        std::string utf8_log = prog->log().toUtf8().constData();
        std::cerr << "OpenGL errors:" << utf8_log << "\n";
        return false;
    }
    if (!prog->addShader(&fShader)) {
        std::cerr << "Unable to add fragment shader to shader program.\n";
        std::string utf8_log = prog->log().toUtf8().constData();
        std::cerr << "OpenGL errors:" << utf8_log << "\n";
        return false;
    }
    if (!prog->link()) {
        std::cerr << "Unable to link shader program.\n";
        std::string utf8_log = prog->log().toUtf8().constData();
        std::cerr << "GLSL Linker errors:" << utf8_log << "\n";
        return false;
    }
    if (!prog->bind()) {
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
    std::vector<GLfloat> terrainTypeIndices;    //these are integer indices, but stored in a float to be passed to GLSL
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
                
                //Each vertex of this quad has the same terrain texture
                GLfloat terrainIndex = static_cast<GLfloat>(subject->getLocation(
                        board::coord(j,i,subject,board::coord::system::globalOrthoLattice))->getTerrain());

                //upper left coord
                GLfloat ULx = (j - (rowParity)/2.0) * locHorizSpacing - locHorizSpacing/2.0;
                GLfloat ULy = i * locVertSpacing + locHorizSpacing/2.0;
                GLfloat ULz = 0.0;
                vertexCoords.push_back(ULx);
                vertexCoords.push_back(ULy);
                vertexCoords.push_back(ULz);
                vertexUVs.push_back(0.0);   vertexUVs.push_back(1.0);
                terrainTypeIndices.push_back(terrainIndex);
                
                //lower left coord
                GLfloat LLx = (j - (rowParity)/2.0) * locHorizSpacing - locHorizSpacing/2.0;
                GLfloat LLy = i * locVertSpacing - locHorizSpacing/2.0;
                GLfloat LLz = 0.0;
                vertexCoords.push_back(LLx);
                vertexCoords.push_back(LLy);
                vertexCoords.push_back(LLz);
                vertexUVs.push_back(0.0);   vertexUVs.push_back(0.0);
                terrainTypeIndices.push_back(terrainIndex);

                //upper right coord
                GLfloat URx = (j - (rowParity)/2.0) * locHorizSpacing + locHorizSpacing/2.0;
                GLfloat URy = i * locVertSpacing + locHorizSpacing/2.0;
                GLfloat URz = 0.0;
                vertexCoords.push_back(URx);
                vertexCoords.push_back(URy);
                vertexCoords.push_back(URz);
                vertexUVs.push_back(1.0);   vertexUVs.push_back(1.0);
                terrainTypeIndices.push_back(terrainIndex);
                
                //lower right coord
                GLfloat LRx = (j - (rowParity)/2.0) * locHorizSpacing + locHorizSpacing/2.0;
                GLfloat LRy = i * locVertSpacing - locHorizSpacing/2.0;
                GLfloat LRz = 0.0;
                vertexCoords.push_back(LRx);
                vertexCoords.push_back(LRy);
                vertexCoords.push_back(LRz);
                vertexUVs.push_back(1.0);   vertexUVs.push_back(0.0);
                terrainTypeIndices.push_back(terrainIndex);
                
                if (verbose) {
                    std::cout << "\tAdding vertices (" << LLx << ", " << LLy
                            << ", " << LLz << "), (" << ULx << ", " << ULy
                            << ", " << ULz << "), (" << LRx << ", " << LRy
                            << ", " << LRz << "), (" << URx << ", " << URy
                            << ", " << URz << "),\twith textureID: " << terrainIndex
                            << ";\n";
                }
            }
        }
    }
    catch (const std::bad_alloc& ex) {
        std::cerr << "\tBad allocation in constructVertexBuffers. The board"
                << " is too large. Location vertex buffer not constructed.\n";
        return false;
    }
    locationVertexBuffer.create();
    locationVertexBuffer.bind();
    locationVertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    locationVertexBuffer.allocate(&vertexCoords[0], vertexCoords.size()*sizeof(vertexCoords[0]));
    locationVertexBuffer.release();
    
    locationUVBuffer.create();
    locationUVBuffer.bind();
    locationUVBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    locationUVBuffer.allocate(&vertexUVs[0], vertexUVs.size()*sizeof(vertexUVs[0]));
    locationUVBuffer.release();
    
    locationTerrainTypeBuffer.create();
    locationTerrainTypeBuffer.bind();
    locationTerrainTypeBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    locationTerrainTypeBuffer.allocate(&terrainTypeIndices[0]
            , terrainTypeIndices.size() * sizeof(terrainTypeIndices[0]));
    locationTerrainTypeBuffer.release();
    
    //construct an element buffer for all the quads
    if (verbose) std::cout << "Constructing locationIndexBuffer.\n";
    std::vector<GLushort> vertexIndices;
    GLushort currIndex = 0;
    try {
        if (verbose) std::cout << "\tWriting vertex indices:\n";
        for (int i = 0; i < subject->getNumRows(); ++i) {
            int rowParity = i%2;
            if (verbose) std::cout << "\ti = " << i;
            for (int j = 0; j < subject->getNumCols() + rowParity; ++j) {
                if (verbose) std::cout << "\n\t\tj = " << j << " of " << subject->getNumCols() + rowParity << ":\t";
                for (int twice = 0; twice < 2; ++twice) {   //two triangles make a quad
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
        std::cerr << "\tBad allocation in constructGLBuffers. The board"
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
        std::cout << "\tlocationIndexBuffer contains "
                << 6*numQuads
                << " indices (with corresponding terrain indices): ";
        for (int j = 0; j < 6*numQuads; ++j) {
            std::cout << vertexIndices[j] << ":" << terrainTypeIndices[vertexIndices[j]] << " ";
        }
        std::cout << '\n';
        std::cout.flush();
    }
    
    
    //Same thing for connectionVertexBuffer
    //TODO: add conditions for these to only draw with appropriate colors and
    //when rail exists
    vertexCoords.clear();
    vertexUVs.clear();
    std::vector<GLfloat> isSlashIndex;    //these are integer indices, but stored in a float to be passed to GLSL
    if (verbose) std::cout << "Constructing connectionVertexBuffer.\n";
    try {
        for (int i = 0; i < subject->getNumRows(); ++i) {
            int rowParity = i%2;
            for (int j = 0; j < subject->getNumCols() + rowParity; ++j) {
                //The formulas for these coordinates have two parts: the part
                //with the i or j puts the center in the right place; the other
                //term moves to where the vertex should be relative to the
                //center. Note that rowParity is because every other row is
                //shifted 50% in order to make a hex grid.
                
                //Each vertex of this quad has the same isSlash index (the quad is either a dash or a slash)
                GLfloat isSlash;
                board::coord pt(j, i, subject,
                        board::coord::system::globalOrthoLattice);
                
                //NW first
                if(subject->getLocation(pt)->neighborExists(direction::NW)) {
                    isSlash = 1.0;
                    //upper left coord
                    GLfloat ULx = (j - (rowParity)/2.0) * locHorizSpacing - locHorizSpacing / 2.0 - connSlashOverwidth;
                    GLfloat ULy = i * locVertSpacing + locVertSpacing + connSlashOverheight;
                    GLfloat ULz = 0.0;
                    vertexCoords.push_back(ULx);
                    vertexCoords.push_back(ULy);
                    vertexCoords.push_back(ULz);
                    //the first UV is inverted for all of these b/c slash is horizontally flipped from what texture has for NW
                    //TODO: These are reflipped, possibly due to coordinate system mismatch?
                    vertexUVs.push_back(0.0);   vertexUVs.push_back(1.0);
                    isSlashIndex.push_back(isSlash);

                    //lower left coord
                    GLfloat LLx = (j - (rowParity)/2.0) * locHorizSpacing - locHorizSpacing / 2.0 - connSlashOverwidth;
                    GLfloat LLy = i * locVertSpacing - connSlashOverheight;
                    GLfloat LLz = 0.0;
                    vertexCoords.push_back(LLx);
                    vertexCoords.push_back(LLy);
                    vertexCoords.push_back(LLz);
                    vertexUVs.push_back(0.0);   vertexUVs.push_back(0.0);
                    isSlashIndex.push_back(isSlash);

                    //upper right coord
                    GLfloat URx = (j - (rowParity)/2.0) * locHorizSpacing + connSlashOverwidth;
                    GLfloat URy = i * locVertSpacing + locVertSpacing + connSlashOverheight;
                    GLfloat URz = 0.0;
                    vertexCoords.push_back(URx);
                    vertexCoords.push_back(URy);
                    vertexCoords.push_back(URz);
                    vertexUVs.push_back(1.0);   vertexUVs.push_back(1.0);
                    isSlashIndex.push_back(isSlash);

                    //lower right coord
                    GLfloat LRx = (j - (rowParity)/2.0) * locHorizSpacing + connSlashOverwidth;
                    GLfloat LRy = i * locVertSpacing - connSlashOverheight;
                    GLfloat LRz = 0.0;
                    vertexCoords.push_back(LRx);
                    vertexCoords.push_back(LRy);
                    vertexCoords.push_back(LRz);
                    vertexUVs.push_back(1.0);   vertexUVs.push_back(0.0);
                    isSlashIndex.push_back(isSlash);
                }
                
                //NE second
                if(subject->getLocation(pt)->neighborExists(direction::NE)) {
                    isSlash = 1.0;
                    //upper left coord
                    GLfloat ULx = (j - (rowParity)/2.0) * locHorizSpacing - connSlashOverwidth;
                    GLfloat ULy = i * locVertSpacing + locVertSpacing + connSlashOverheight;
                    GLfloat ULz = 0.0;
                    vertexCoords.push_back(ULx);
                    vertexCoords.push_back(ULy);
                    vertexCoords.push_back(ULz);
                    //TODO: These are reflipped, possibly due to coordinate system mismatch?
                    vertexUVs.push_back(1.0);   vertexUVs.push_back(1.0);
                    isSlashIndex.push_back(isSlash);

                    //lower left coord
                    GLfloat LLx = (j - (rowParity)/2.0) * locHorizSpacing - connSlashOverwidth;
                    GLfloat LLy = i * locVertSpacing - connSlashOverheight;
                    GLfloat LLz = 0.0;
                    vertexCoords.push_back(LLx);
                    vertexCoords.push_back(LLy);
                    vertexCoords.push_back(LLz);
                    vertexUVs.push_back(1.0);   vertexUVs.push_back(0.0);
                    isSlashIndex.push_back(isSlash);

                    //upper right coord
                    GLfloat URx = (j - (rowParity)/2.0) * locHorizSpacing + locHorizSpacing / 2.0 + connSlashOverwidth;
                    GLfloat URy = i * locVertSpacing + locVertSpacing + connSlashOverheight;
                    GLfloat URz = 0.0;
                    vertexCoords.push_back(URx);
                    vertexCoords.push_back(URy);
                    vertexCoords.push_back(URz);
                    vertexUVs.push_back(0.0);   vertexUVs.push_back(1.0);
                    isSlashIndex.push_back(isSlash);

                    //lower right coord
                    GLfloat LRx = (j - (rowParity)/2.0) * locHorizSpacing + locHorizSpacing/2.0 + connSlashOverwidth;
                    GLfloat LRy = i * locVertSpacing - connSlashOverheight;
                    GLfloat LRz = 0.0;
                    vertexCoords.push_back(LRx);
                    vertexCoords.push_back(LRy);
                    vertexCoords.push_back(LRz);
                    vertexUVs.push_back(0.0);   vertexUVs.push_back(0.0);
                    isSlashIndex.push_back(isSlash);
                }
                
                //E third
                if(subject->getLocation(pt)->neighborExists(direction::E)) {
                    isSlash = 0.0;
                    //upper left coord
                    GLfloat ULx = (j - (rowParity)/2.0) * locHorizSpacing - connDashOverwidth;
                    GLfloat ULy = i * locVertSpacing + connDashHeight;
                    GLfloat ULz = 0.0;
                    vertexCoords.push_back(ULx);
                    vertexCoords.push_back(ULy);
                    vertexCoords.push_back(ULz);
                    vertexUVs.push_back(0.0);   vertexUVs.push_back(1.0);
                    isSlashIndex.push_back(isSlash);

                    //lower left coord
                    GLfloat LLx = (j - (rowParity)/2.0) * locHorizSpacing - connDashOverwidth;
                    GLfloat LLy = i * locVertSpacing - connDashHeight;
                    GLfloat LLz = 0.0;
                    vertexCoords.push_back(LLx);
                    vertexCoords.push_back(LLy);
                    vertexCoords.push_back(LLz);
                    vertexUVs.push_back(0.0);   vertexUVs.push_back(0.0);
                    isSlashIndex.push_back(isSlash);

                    //upper right coord
                    GLfloat URx = (j - (rowParity)/2.0) * locHorizSpacing + locHorizSpacing + connSlashOverwidth;
                    GLfloat URy = i * locVertSpacing + connDashHeight;
                    GLfloat URz = 0.0;
                    vertexCoords.push_back(URx);
                    vertexCoords.push_back(URy);
                    vertexCoords.push_back(URz);
                    vertexUVs.push_back(1.0);   vertexUVs.push_back(1.0);
                    isSlashIndex.push_back(isSlash);

                    //lower right coord
                    GLfloat LRx = (j - (rowParity)/2.0) * locHorizSpacing + locHorizSpacing + connSlashOverwidth;
                    GLfloat LRy = i * locVertSpacing - connDashHeight;
                    GLfloat LRz = 0.0;
                    vertexCoords.push_back(LRx);
                    vertexCoords.push_back(LRy);
                    vertexCoords.push_back(LRz);
                    vertexUVs.push_back(1.0);   vertexUVs.push_back(0.0);
                    isSlashIndex.push_back(isSlash);
                }
                
                if (verbose) {
                    std::cout << "\tAdding connection textures from vertex ("
                            << j << "," << i << ")\n";
                }
            }
        }
    }
    catch (const std::bad_alloc& ex) {
        std::cerr << "\tBad allocation in connectionVertexBuffer. The board"
                << " is too large. Connection vertex buffer not constructed.\n";
        std::cerr << "Warning: Partial construction of GLBuffers.\n";
        return false;
    }
    
    connectionVertexBuffer.create();
    connectionVertexBuffer.bind();
    connectionVertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    connectionVertexBuffer.allocate(&vertexCoords[0], vertexCoords.size()*sizeof(vertexCoords[0]));
    connectionVertexBuffer.release();
    
    connectionUVBuffer.create();
    connectionUVBuffer.bind();
    connectionUVBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    connectionUVBuffer.allocate(&vertexUVs[0], vertexUVs.size()*sizeof(vertexUVs[0]));
    connectionUVBuffer.release();
    
    connectionDashSlashBuffer.create();
    connectionDashSlashBuffer.bind();
    connectionDashSlashBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    connectionDashSlashBuffer.allocate(&isSlashIndex[0]
            , isSlashIndex.size() * sizeof(isSlashIndex[0]));
    connectionDashSlashBuffer.release();
    
    //TODO: Vertex indices for connections
    //construct an element buffer for all the quads
    if (verbose) std::cout << "Constructing connectionIndexBuffer.\n";
    vertexIndices.clear();
    currIndex = 0;
    try {
        if (verbose) std::cout << "\tWriting connection vertex indices:\n";
        for (int i = 0; i < subject->getNumRows(); ++i) {
            int rowParity = i%2;
            if (verbose) std::cout << "\ti = " << i;
            for (int j = 0; j < subject->getNumCols() + rowParity; ++j) {
                if (verbose) std::cout << "\n\t\tj = " << j << " of " << subject->getNumCols() + rowParity << ":\t";
                board::coord pt(j, i, subject,
                        board::coord::system::globalOrthoLattice);
                
                for (direction dir = direction::E; dir != direction::W; ++dir) {
                    if (!subject->getLocation(pt)->neighborExists(dir))
                        continue;   //Skip if there's no neighbor in this direction
                    for (int twice = 0; twice < 2; ++twice) {   //quads have 2 triangles
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
            }
            if (verbose) std::cout << '\n';
        }
    }
    catch (const std::bad_alloc& ex) {
        std::cerr << "\tBad allocation in constructGLBuffers. The board"
                << " is too large. Location element buffers not constructed.\n";
        std::cerr << "Warning: Partial construction of GLBuffers.\n";
        return false;
    }
     
    //Note: connectionIndexBuffer was constructed as an index buffer
    connectionIndexBuffer.create();
    connectionIndexBuffer.bind();
    connectionIndexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    connectionIndexBuffer.allocate(&vertexIndices[0] 
            , sizeof(vertexIndices[0]) * vertexIndices.size());
    connectionIndexBuffer.release();
        
    if(verbose) {
        std::cout << "\tconnectionIndexBuffer contains "
                << vertexIndices.size()
                << " indices: ";
        for (int j = 0; j < vertexIndices.size(); ++j) {
            std::cout << vertexIndices[j] << " ";
        }
        std::cout << '\n';
        std::cout.flush();
    }
    
    return true;
}

void boardWindow::loadTerrainTextureFilenames(std::istream& in) {
    //TODO: Note that the file names must be given in the same order as the enum
    using dictPair = std::pair<terrain, std::string>;
    for (terrain t = EnumTraits<terrain>::FIRST;
            t != EnumTraits<terrain>::LOGICAL_LAST;
            ++t) {
        std::string line;
        if (!std::getline(in, line))
            return; //No more input, all available texture filenames set
        terrainTextureFilenames.insert(dictPair(t, line));
    }
}

void boardWindow::initializeTextureAtlas(QOpenGLTexture& atlas,
        std::size_t layers,
        std::function<std::string(int)> getFilenameFunc)
        //std::string (*getFilenameFunc)(int) ) //This function should throw std::out_of_range if passed an int w/out a corresponding filename
{
    QImage fileLoadImage;
    QImage GLFormatImage;
    std::string filename;
    atlas.setLayers(layers);
    QOpenGLPixelTransferOptions transferOptions;
    transferOptions.setAlignment(1);
    
    //The texture at index 0 is the default and *must* exist; other layers can
    //be initialized now or later (or never)
    try {
        filename = getFilenameFunc(0);
    }
    catch (std::out_of_range ex) {
        std::cerr << "Warning: No filename for default texture for a texture "
                << "atlas. Aborting texture load.\n";
        return;
    }
    if (!fileLoadImage.load(filename.c_str())) {
        std::cerr << "Failed to load texture image '" << filename << "'\n";
        std::cerr << "This was the default texture of an atlas; aborting "
                << " construction of this atlas.\n";
        return;
    }
    GLFormatImage = fileLoadImage.convertToFormat(QImage::Format_RGBA8888);
    
    //Based on Qt source code for setData(QImage)
    if (context->isOpenGLES() && context->format().majorVersion() < 3)
        atlas.setFormat(QOpenGLTexture::RGBAFormat);
    else
        atlas.setFormat(QOpenGLTexture::RGBA8_UNorm);
    atlas.setSize(GLFormatImage.width(), GLFormatImage.height());
    atlas.setMipLevels(atlas.maximumMipLevels());
    atlas.allocateStorage(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8);
    atlas.setData(0, 0, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, 
            GLFormatImage.constBits(), &transferOptions);
    
    for (int i = 1; i < atlas.layers(); ++i) {
        try {
            filename = getFilenameFunc(i);
        }
        catch (std::out_of_range ex) {
            std::cerr << "Warning: No texture image filename for atlas layer "
                    << i << "\n";
            continue;
        }
        if (!fileLoadImage.load(filename.c_str())) {
            std::cerr << "Failed to load texture image '" << filename << "'\n";
        }
        GLFormatImage = fileLoadImage.convertToFormat(QImage::Format_RGBA8888);
        atlas.setData(0, i, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                GLFormatImage.constBits(), &transferOptions);
    }
    
    if (verbose) {
        std::cout << "Loaded texture atlas with the"
                << " following properties:\t";
        std::cout << "Height: " << atlas.height() << '\t';
        std::cout << "Width: " << atlas.width() << '\t';
        std::cout << "Layers: " << atlas.layers() << '\n';
    }
}

void boardWindow::loadTerrainTextures() {
    std::function<std::string(int)> getTerrainTexFilenameFromIndex = [this](int i) -> std::string {
        return terrainTextureFilenames.at(static_cast<terrain>(i));
    };
    
    //TODO: Next func is temporary
    std::function<std::string(int)> getConnectionTexFilenameFromIndex = [](int i) -> std::string {
        std::string name;
        if (i == 0) { name = ":/connDash.png"; return name; }
        if (i == 1) { name = ":/connSlash.png"; return name; }
        throw (new std::out_of_range(
                std::string("No filename for index ") + std::to_string(i) + '\n'
                ));
    };
    
    initializeTextureAtlas(terrainTextureAtlas,
            Enum::count<terrain>(),
            getTerrainTexFilenameFromIndex
            );

    initializeTextureAtlas(connectionTextureAtlas,
            2,
            getConnectionTexFilenameFromIndex
            );
}

