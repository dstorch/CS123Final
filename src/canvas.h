#pragma once

#include <QGLWidget>
#include <QMenuBar>

#include "camera.h"
#include "vector.h"

typedef Vector2 GridPoint;

class Lab5Canvas : public QGLWidget
{
    Q_OBJECT

public:

    Lab5Canvas(QWidget *parent = 0);
    virtual ~Lab5Canvas();

protected:

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

    //Member variables

    /** Contains the current position and orientation of the virtual camera */
    Camera *m_camera;
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
