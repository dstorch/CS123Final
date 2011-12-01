#pragma once

#include <QGLWidget>
#include <QMenuBar>

#include "labcamera.h"
#include "vector.h"

typedef Vector2 GridPoint;

class Lab5Canvas : public QGLWidget
{
    Q_OBJECT

public:

    Lab5Canvas(QWidget *parent = 0);
    virtual ~Lab5Canvas();

protected:

    // Helper methods


    // Methods you will have to fill in

    /**
     * You need to fill this in.
     *
     * Computes the normal vector of each terrain vertex and stores it in the corresponding vertex.
     */
    void computeNormals();
    /**
     * You need to fill this in.
     *
     * Renders the terrain to the canvas. This is where you should make all of your OpenGL calls.
     */
    virtual void paintGL();

    // Other Methods

    /**
     * Called whenever m_camera is modified or the canvas is resized. Sets the current OpenGL projection
     * and modelview matrices to match the values in m_camera.
     */
    void updateCamera();

    /** Sets the initial values of OpenGL state variables used by this lab */
    void initializeGL();
    /** Updates the current OpenGL state to avoid object distortion when the window is resized. */
    void resizeGL(int w, int h);

    /** Allows this canvas to handle mouse input */
    void mousePressEvent(QMouseEvent *event);
    /** Allows this canvas to handle mouse input */
    void mouseMoveEvent(QMouseEvent *event);
    /** Allows this canvas to handle keyboard input */
    void keyPressEvent(QKeyEvent *event);
    /** Called by the UI when sidebar settings are changed */
    virtual void settingsChanged();

    /**
     * Sets default values for the four corners of the terrain grid and calls subdivideSquare()
     * to begin the terrain generation process. You do not need to modify this function.
     */
    void populateTerrain();
    /**
     * Draws a line at each vertex showing the direction of that vertex's normal. You may find
     * this to be a useful tool if you're having trouble getting the lighting to look right.
     * By default, this function is called in paintGL(), but only renders anything if
     * m_renderNormals is true. You do not need to modify this function.
     */
    void drawNormals();

    //Member variables

    /** Contains the position of the vertex at each grid point in the terrain */
    Vector3 *m_terrain;
    /** Contains the normal vector of the vertex at each grid point in the terrain */
    Vector3 *m_normalMap;
    /** The number of grid points on one side of the square terrain grid. */
    GLint m_gridLength;
    /** The number of recursion levels to use to generate terrain. Can be used as a level-of-detail parameter. */
    GLint m_depth;
    /** Controls the height of the terrain. Large values generate higher peaks and lower valleys. */
    GLfloat m_roughness;
    /** Controls how much heights can vary per recursion depth level. Higher values generate smoother terrain. */
    GLint m_decay;
    /** Whether or not drawNormals() should do anything when called. */
    bool m_renderNormals;

    /** Contains the current position and orientation of the virtual camera */
    LabCamera *m_camera;
    /** Causes the canvas to be redrawn at a regular interval */
    QTimer *m_timer;
    /** How often the canvas is redrawn, in frames per second */
    float m_fps;
    /** Helper for implementing trackball rotation */
    Vector2 m_lastMousePos;

protected slots:
     void mouseDragEvent(QMouseEvent *event);
     void redraw();

 signals:
      void _mouseDrag(QMouseEvent *);
};
