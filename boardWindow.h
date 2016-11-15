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

#include <vector>
#include <map>
#include <string>

#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLDebugLogger>
#include <QtGui/QOpenGLTexture>

#include <qopengl.h>

class board;
enum class terrain;

class boardWindow : public QWindow, protected QOpenGLFunctions {
    Q_OBJECT
public:
    explicit boardWindow(QWindow* parent = 0);
    boardWindow(const boardWindow&) = delete;
    boardWindow(const boardWindow&&) = delete;
    virtual ~boardWindow();
    
    void setFormat() { QWindow::setFormat(surfaceFormat); };
    void setAnimating(bool val) { 
        animating = val;
        if (animating) renderLater();
    };
    
    bool updateShaders(std::string vertShaderFile, std::string fragShaderFile,
            QOpenGLShaderProgram* prog);
public slots:
    void render();
    void renderLater();
protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    void exposeEvent(QExposeEvent *event) Q_DECL_OVERRIDE;
    
    bool constructGLBuffers();
private:
    board* subject;
    bool animating;
    bool updatePending;
    bool verbose;
    int zoomLevel;  //Number of degrees of scrolling above/below neutral; controls zoomFactor
    double zoomFactor;
    double zoomFactorFromLevel(int level);
    QSurfaceFormat surfaceFormat;
    QOpenGLContext *context;
    QOpenGLDebugLogger *debugLogger;
    QRectF view;
    GLuint projMatrixHandle;
    QOpenGLTexture terrainTextureAtlas;
    
    QOpenGLBuffer locationVertexBuffer;
    QOpenGLBuffer locationUVBuffer;
    QOpenGLBuffer locationTerrainTypeBuffer;
    QOpenGLBuffer locationIndexBuffer;
    QOpenGLBuffer connectionVertexBuffer;
    QOpenGLBuffer connectionUVBuffer;
    QOpenGLBuffer connectionDashSlashBuffer;
    QOpenGLBuffer connectionIndexBuffer;
    std::map<terrain, std::string> terrainTextureFilenames;
    
    void initGL();
    void loadTerrainTextureFilenames(std::istream& in);
    void loadTerrainTextures();
    void printDebugLog();
    
    //Shaders
    bool createShaderProgram(QOpenGLShaderProgram* prog);     //return whether successfully created. If false, the old shader program (if any) is retained in the shader program
    std::string vertexShaderFilename;
    std::string fragmentShaderFilename;
    QOpenGLShaderProgram shaderProgram;
    QOpenGLShaderProgram fixedColorShaderProgram;
    
    /*
     * Graphical parameters
     */
    // Things like how far apart vertices are
    const GLfloat locHorizSpacing;
    const GLfloat locVertSpacing;
    //const GLfloat vertexIconWidth == locHorizSpacing
    //how far the connection quads extend beyond their endpoints
    const GLfloat connSlashOverwidth;
    const GLfloat connSlashOverheight;
    const GLfloat connDashOverwidth;
    const GLfloat connDashHeight;
};

#endif /* BOARDWINDOW_H */

