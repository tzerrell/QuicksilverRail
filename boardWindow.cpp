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
        //, view(-40, -60, 192, 144)
        , locHorizSpacing(20.0f)
        , locVertSpacing(14.0f)
        , verbose(true)
        , terrainTextureAtlas(QOpenGLTexture::Target2DArray)
        , locationIndexBuffer(QOpenGLBuffer::IndexBuffer)
{
    subject = new board;    //TODO: do this in a reasonable way ...
    surfaceFormat.setSamples(4);
    setSurfaceType(QWindow::OpenGLSurface);
    
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

void boardWindow::initGL() {
    //TODO: debug code
    subject->setLocationTerrain(1,1, static_cast<terrain>(1));
    //TODO: end debug
    
    
    updateShaders("generic.vertexshader", "generic.fragmentshader");    //TODO: adjust to allow custom filenames
    projMatrixHandle = glGetUniformLocation(shaderProgram.programId(), "projectionMat");
    constructGLBuffers();
    loadTerrainTextures();
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
    //TODO: do this in a real way
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
    
    
    std::vector<int> texHandleIDs;  //TODO: Want this?
    terrainTextureAtlas.bind(13); //TODO
    if (!terrainTextureAtlas.isBound()) {
        std::cerr << "Error binding terrain texture atlas.\n";
    }
    int TODOTempUnifValArray[1] = { 13 };
    shaderProgram.setUniformValueArray("TODOTestSampler", TODOTempUnifValArray, 1);
    //shaderProgram.setUniformValueArray("TODOTestSampler", &texHandleIDs[0], Enum::count<terrain>());
    
    int numQuads = subject->getNumRows() * subject->getNumCols() + subject->getNumRows()/2;
    locationIndexBuffer.bind();
    glDrawElements(GL_TRIANGLES, numQuads * 6, GL_UNSIGNED_SHORT, 0);
    locationIndexBuffer.release();
    terrainTextureAtlas.release();
    locationVertexBuffer.release();
    //TODO: End of test render
    
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    
    glDisable(GL_BLEND);
    
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
                    std::cout << "\tAdding vertices (" << LLx << ", " << LLy
                            << ", " << LLz << "), (" << ULx << ", " << ULy
                            << ", " << ULz << "), (" << LRx << ", " << LRy
                            << ", " << LRz << "), (" << URx << ", " << URy
                            << ", " << URz << ")\n";
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
    
    //construct an element buffer for all the quads
    if (verbose) std::cout << "Constructing locationIndexBuffer.\n";
    std::vector<GLushort> vertexIndices;
    std::vector<GLfloat> terrainTypeIndices;    //these are integer indices, but stored in a float to be passed to GLSL
    GLushort currIndex = 0;
    try {
        if (verbose) std::cout << "\tWriting vertex indices:\n";
        for (int i = 0; i < subject->getNumRows(); ++i) {
            int rowParity = i%2;
            if (verbose) std::cout << "\ti = " << i;
            for (int j = 0; j < subject->getNumCols() + rowParity; ++j) {
                if (verbose) std::cout << "\n\t\tj = " << j << " of " << subject->getNumCols() + rowParity << ":\t";
                GLfloat terrainIndex = static_cast<GLfloat>(subject->getLocation(j,i,true)->getTerrain());
                for (int twice = 0; twice < 2; ++twice) {
                    if (verbose) {
                        std::cout << currIndex << ' ' << currIndex + 1 << ' '
                                << currIndex + 2 << ' ';
                    }
                    vertexIndices.push_back(currIndex);
                    terrainTypeIndices.push_back(terrainIndex);
                    vertexIndices.push_back(currIndex + 1);
                    terrainTypeIndices.push_back(terrainIndex);
                    vertexIndices.push_back(currIndex + 2);
                    terrainTypeIndices.push_back(terrainIndex);
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
    
    locationTerrainTypeBuffer.create();
    locationTerrainTypeBuffer.bind();
    locationTerrainTypeBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    locationTerrainTypeBuffer.allocate(&terrainTypeIndices[0]
            , sizeof(terrainTypeIndices[0]) * 6 * numQuads);
    locationTerrainTypeBuffer.release();
        
    if(verbose) {
        std::cout << "\tlocationIndexBuffer contains "
                << 6*numQuads
                << " indices (with corresponding terrain indices): ";
        for (int j = 0; j < 6*numQuads; ++j) {
            std::cout << vertexIndices[j] << ":" << terrainTypeIndices[j] << " ";
        }
        std::cout << '\n';
        std::cout.flush();
    }
    
    //TODO: Same thing for connectionVertexBuffer
    
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

void boardWindow::loadTerrainTextures() {
    QImage image;
    QImage glImage;
    std::string filename;
    terrainTextureAtlas.setLayers(Enum::count<terrain>());
    QOpenGLPixelTransferOptions transferOptions;
    transferOptions.setAlignment(1);
    
    //First setup the default terrain texture
    try {
        filename = terrainTextureFilenames.at(static_cast<terrain>(0));
    }
    catch (std::out_of_range ex) {
        std::cerr << "Warning: No texture image filename for terrain type "
                << static_cast<int>(0) << "\n";
        std::cerr << "This is the default terrain, which must have a filename "
                << "to load terrain textures. Aborting load.\n";
        return;
    }
    if (!image.load(filename.c_str())) {
        std::cerr << "Failed to load texture image '" << filename << "'\n";
        std::cerr << "This was the default terrain texture; aborting terrain"
                << " texture construction.\n";
        return;
    }
    glImage = image.convertToFormat(QImage::Format_RGBA8888);
    
    //Based on Qt source code for setData(QImage)
    if (context->isOpenGLES() && context->format().majorVersion() < 3)
        terrainTextureAtlas.setFormat(QOpenGLTexture::RGBAFormat);
    else
        terrainTextureAtlas.setFormat(QOpenGLTexture::RGBA8_UNorm);
    terrainTextureAtlas.setSize(image.width(), image.height());
    terrainTextureAtlas.setMipLevels(terrainTextureAtlas.maximumMipLevels());
    terrainTextureAtlas.allocateStorage(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8);
    terrainTextureAtlas.setData(0, 0, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, glImage.constBits(), &transferOptions);
    
    for (int i = 1; i < terrainTextureAtlas.layers(); ++i) {
        terrain t = static_cast<terrain>(i);
        try {
            filename = terrainTextureFilenames.at(t);
        }
        catch (std::out_of_range ex) {
            std::cerr << "Warning: No texture image filename for terrain type "
                    << static_cast<int>(t) << "\n";
            continue;
        }
        if (!image.load(filename.c_str())) {
            std::cerr << "Failed to load texture image '" << filename << "'\n";
        }
        glImage = image.convertToFormat(QImage::Format_RGBA8888);
        terrainTextureAtlas.setData(0, i, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                glImage.constBits(), &transferOptions);
    }
    
    //TODO: Debug info
    if (verbose) {
        std::cout << "Loaded terrain textures with the"
                << " following properties:\n";
        std::cout << "\tHeight: " << terrainTextureAtlas.height() << '\n';
        std::cout << "\tWidth: " << terrainTextureAtlas.width() << '\n';
        std::cout << "\tLayers: " << terrainTextureAtlas.layers() << '\n';
    }
}

