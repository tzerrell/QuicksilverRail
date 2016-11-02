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
        , locationVertexBufferID(0)
        , locationStripElementBufferIDs(nullptr)
        , locHorizSpacing(20.0f)
        , locVertSpacing(14.0f)
{
    surfaceFormat.setSamples(4);
    setSurfaceType(QWindow::OpenGLSurface);
    
    subject = new board;    //TODO: construct this in a smarter way; or at least do so in board
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
        constructGLBuffers();
        updateShaderFiles("generic.vertexshader", "generic.fragmentshader");
        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    }
    
    const qreal pixelRatio = devicePixelRatio();
    glViewport(0, 0, width() * pixelRatio, height() * pixelRatio);
    
    QMatrix4x4 renderMatrix;
    renderMatrix.ortho(view);
    
    //TODO
    drawBoard();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //TODO: render scene
    
    
    context->swapBuffers(this);
    
    if (animating) renderLater();
}

void boardWindow::drawBoard() {
    glBindBuffer(GL_ARRAY_BUFFER, locationVertexBufferID);
    glVertexAttribPointer(
       0,                  //TODO: make significant
       3,                  // size
       GL_FLOAT,           // type
       GL_FALSE,           // normalized?
       0,                  // stride
       (void*)0            // array buffer offset
    );
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, locationStripElementBufferIDs[0]);    //TODO: Program SegFaults with 0x0 in the pointer locationStripElementBufferIDs
    glDrawElements(
        GL_TRIANGLE_STRIP,  /* mode */
        5,                  /* count */
        GL_UNSIGNED_SHORT,  /* type */
        (void*)0            /* element array buffer offset */
    );
    
    
    //TODO: Needs to be more thoroughly implemented
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

bool boardWindow::constructGLBuffers() {
    //construct a vertex buffer for the location icon quadrilaterals
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
                << " is too large. Location vertex buffers not constructed.\n";
        return false;
    }
    
    glDeleteBuffers(1, &locationVertexBufferID);    //if there are any previous contents, delete them
    glGenBuffers(1, &locationVertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, locationVertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertexCoords.size() * sizeof(GLfloat),
            &vertexCoords[0], GL_STATIC_DRAW);
    
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
    
    //TODO: need to delete and null out locationStripElementBufferIDs whenever
    //the size of the board is changed, as we can't do that here (won't know
    //how many rows there are & thus won't know how many buffers to delete).
    if (locationStripElementBufferIDs != nullptr) {
        glDeleteBuffers(subject->getNumRows(), locationStripElementBufferIDs);
        delete[] locationStripElementBufferIDs;
    }
    locationStripElementBufferIDs = new GLuint[subject->getNumRows()];
    glGenBuffers(subject->getNumRows(), locationStripElementBufferIDs);
    for (int i = 0; i < subject->getNumRows(); ++i) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, locationStripElementBufferIDs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                (stripStartIndices[i+1] - stripStartIndices[i])*sizeof(GLushort), 
                &vertexIndices[stripStartIndices[i]], GL_STATIC_DRAW);
    }
    
    //TODO: Same thing for connectionVertexBuffer
    
    return true;
}


bool boardWindow::updateShaderFiles(std::string vertFile, std::string fragFile) {
    vertexShaderFilename = vertFile;
    fragmentShaderFilename = fragFile;
    return createShaderProgram();
}

bool boardWindow::createShaderProgram() {
    std::ostringstream vertShaderCodeStream;
    std::string vertShaderCode;
    std::ostringstream fragShaderCodeStream;
    std::string fragShaderCode;
    std::ifstream fin;
    
    if (verbose) {
        std::cout << "Creating GLSL shader program from vertex shader file \""
                << vertexShaderFilename << "\" and fragment shader file \""
                << fragmentShaderFilename << "\"\n";
    }
    
    //Read shader code files into strings
    fin.open(vertexShaderFilename);
    if(fin.is_open()) {
        vertShaderCodeStream << fin.rdbuf();
        vertShaderCode = vertShaderCodeStream.str();
    }
    else {
        std::cerr << "Unable to open vertex shader file "
                << vertexShaderFilename << ". Shader program not created.\n";
        return false;
    }
    fin.close();
    
    fin.open(fragmentShaderFilename);
    if(fin.is_open()) {
        fragShaderCodeStream << fin.rdbuf();
        fragShaderCode = fragShaderCodeStream.str();
    }
    else {
        std::cerr << "Unable to open fragment shader file "
                << vertexShaderFilename << ". Shader program not created.\n";
        return false;
    }
    fin.close();
    
    //Compile Shaders
    GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    int infoLogLength;
    std::vector<char> errorMessage;
    
    if (verbose) std::cout << "Compiling vertex shader\n";
    const char* vertCodePtr = vertShaderCode.c_str();
    glShaderSource(vertShaderID, 1, &vertCodePtr, NULL);    //yes, the & is correct, this requires an array of const char *
    glCompileShader(vertShaderID);
    glGetShaderiv(vertShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if ( infoLogLength > 1 ){
        try {
            errorMessage.reserve(infoLogLength+1);
        }
        catch (const std::length_error& ex) {
            std::cerr << "Vertex shader compilation error. Shader program "
                    << "not created. Shader compiler error log too long to "
                    << "output.\n";
            std::cerr << "Length error in handling previous error: " 
                    << ex.what() << '\n';
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        catch (const std::bad_alloc& ex) {
            std::cerr << "Vertex shader compilation error. Shader program "
                    << "not created. Shader compiler error log too long to "
                    << "output.\n";
            std::cerr << "Bad allocation in handling previous error: " 
                    << ex.what() << '\n';
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        catch (...) {
            std::cerr << "Vertex shader compilation error. Shader program not "
                    << "created. Unexpected exception while handling error.\n";
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        glGetShaderInfoLog(vertShaderID, infoLogLength, &infoLogLength, &errorMessage[0]);
        std::cerr << "Vertex shader compilation error. Shader program "
                << "not created. Shader compiler error log follows "
                << "(Expect " << infoLogLength << " characters):\n"
                << &errorMessage[0] << '\n';
        std::cerr << "Vertex shader code:\n" << vertShaderCode << '\n';
        //cleanup
        glDeleteShader(vertShaderID);
        glDeleteShader(fragShaderID);
        return false;
    }
    
    if (verbose) std::cout << "Compiling fragment shader\n";
    const char* fragCodePtr = fragShaderCode.c_str();
    glShaderSource(fragShaderID, 1, &fragCodePtr, NULL);    //yes, the & is correct, this requires an array of const char *
    glCompileShader(fragShaderID);
    glGetShaderiv(fragShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if ( infoLogLength > 1 ){
        try {
            errorMessage.reserve(infoLogLength+1);
        }
        catch (const std::length_error& ex) {
            std::cerr << "Fragment shader compilation error. Shader program "
                    << "not created. Shader compiler error log too long to "
                    << "output.\n";
            std::cerr << "Length error in handling previous error: " 
                    << ex.what() << '\n';
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        catch (const std::bad_alloc& ex) {
            std::cerr << "Fragment shader compilation error. Shader program "
                    << "not created. Shader compiler error log too long to "
                    << "output.\n";
            std::cerr << "Bad allocation in handling previous error: " 
                    << ex.what() << '\n';
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        catch (...) {
            std::cerr << "Fragment shader compilation error. Shader program not "
                    << "created. Unexpected exception while handling error.\n";
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        glGetShaderInfoLog(fragShaderID, infoLogLength, NULL, &errorMessage[0]);
        std::cerr << "Fragment shader compilation error. Shader program "
                << "not created. Shader compiler error log follows:\n"
                << &errorMessage[0];            
        //cleanup
        glDeleteShader(vertShaderID);
        glDeleteShader(fragShaderID);
        return false;
    }
    
    //Link GLSL program
    if (verbose) std::cout << "Linking GLSL shader program\n";
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertShaderID);
    glAttachShader(programID, fragShaderID);
    glLinkProgram(programID);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if ( infoLogLength > 1 ){
        try {
            errorMessage.reserve(infoLogLength+1);
        }
        catch (const std::length_error& ex) {
            std::cerr << "GLSL linker error. Shader program not "
                    << "not created. Shader compiler error log too long to "
                    << "output.\n";
            std::cerr << "Length error in handling previous error: " 
                    << ex.what() << '\n';
            glDetachShader(programID, vertShaderID);
            glDetachShader(programID, fragShaderID);
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        catch (const std::bad_alloc& ex) {
            std::cerr << "GLSL linker error. Shader program not "
                    << "not created. Shader compiler error log too long to "
                    << "output.\n";
            std::cerr << "Bad allocation in handling previous error: " 
                    << ex.what() << '\n';
            glDetachShader(programID, vertShaderID);
            glDetachShader(programID, fragShaderID);
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        catch (...) {
            std::cerr << "GLSL linker error. Shader program not "
                    << "created. Unexpected exception while handling error.\n";            
            glDetachShader(programID, vertShaderID);
            glDetachShader(programID, fragShaderID);
            glDeleteShader(vertShaderID);
            glDeleteShader(fragShaderID);
            return false;
        }
        glGetProgramInfoLog(programID, infoLogLength, NULL, &errorMessage[0]);
        std::cerr << "GLSL linker error. Shader program not created. Error "
                << "log follows:\n"
                << &errorMessage[0];
        
        //cleanup
        glDetachShader(programID, vertShaderID);
        glDetachShader(programID, fragShaderID);
        glDeleteShader(vertShaderID);
        glDeleteShader(fragShaderID);
        return false;
    }
    
    //cleanup
    glDetachShader(programID, vertShaderID);
    glDetachShader(programID, fragShaderID);
    glDeleteShader(vertShaderID);
    glDeleteShader(fragShaderID);
    glDeleteProgram(programID);  //Delete old shader program (if no previous program, this does nothing, which is correct)
    
    //Set shader program to be what we just created
    shaderProgramID = programID;
    
    return true;
}



