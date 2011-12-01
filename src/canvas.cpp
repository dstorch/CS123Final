#include "canvas.h"

#ifdef __APPLE__
#include <OpenGL/glext.h>
#else
#include <GL/glext.h>
#endif

#include <qgl.h>
#include <iostream>

#include <QMouseEvent>
#include <QTimer>
#include <QFile>

#include <cmath>

using namespace std;

// hack to get around windef.h on Windows
#undef near
#undef far

Lab5Canvas::Lab5Canvas(QWidget *parent) : QGLWidget(parent), m_fps(30.0f)
{
    m_camera = new Camera();
    m_camera->eye.x = 0.0f, m_camera->eye.y = 20.f, m_camera->eye.z = 10.0f;
    m_camera->center.x = 0.0f, m_camera->center.y = 0.0f, m_camera->center.z = 0.0f;
    m_camera->up.x = 0.0f, m_camera->up.y = 1.0f, m_camera->up.z = 0.0f;
    m_camera->angle = 45.0f, m_camera->near = 1.0f, m_camera->far = 1000.0f;

    QObject::connect(this, SIGNAL(_mouseDrag(QMouseEvent*)), this, SLOT(mouseDragEvent(QMouseEvent*)));
    setAutoBufferSwap(false);
    setFocusPolicy(Qt::StrongFocus);
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(redraw()));
    m_timer->start(1000.0f / m_fps);

}

// Destructor
Lab5Canvas::~Lab5Canvas()
{
    if (m_timer)
    {
        delete m_timer;
        m_timer = 0;
    }

    if (m_camera)
    {
        delete m_camera;
        m_camera = 0;
    }
}

/**
 * You need to fill this in.
 *
 * Renders the terrain to the canvas. This is where you should make all of your OpenGL calls.
 */
void Lab5Canvas::paintGL()
{
    // Clear the color and depth buffers to the current glClearColor
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Push a new matrix onto the stack for modelling transformations
    glPushMatrix();

    // test the camera interaction
    glColor3f(1.0, 0.0, 0.0);
    GLUquadricObj *quadric = gluNewQuadric();
    gluCylinder(quadric, 1.0, 0.0, 2.0, 20, 20);

    // Discard the modelling transformations (leaving only camera settings)
    glPopMatrix();
    // Force OpenGL to perform all pending operations -- usually a good idea to call this
    glFlush();
    // Swap the buffers to show what we have just drawn onto the screen
    swapBuffers();
}


/** Sets the initial values of OpenGL state variables used by this lab */
void Lab5Canvas::initializeGL()
{
    glClearColor(0, 0, 0, 0);   // Always reset the screen to black before drawing anything
    glEnable(GL_DEPTH_TEST);    // When drawing a triangle, only keep pixels closer to the camera than what's already been drawn

    // Make things pretty
    glEnable(GL_MULTISAMPLE);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    // Bind the ambient and diffuse color of each vertex to the current glColor() value
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Cull triangles that are facing away from the camera
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Set up a single light
    glEnable(GL_LIGHTING);
    GLfloat ambientColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat diffuseColor[] = { 1.0f, 1.0f, 1.0, 1.0f };
    GLfloat specularColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat lightPosition[] = { 0.f, 0.f, 10.f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHT0);

    // Enable texturing - feel free to substitue with your own image
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

/** Updates the current OpenGL state to avoid object distortion when the window is resized. */
void Lab5Canvas::resizeGL(int w, int h)
{
    // Prevent a divide by zero
    if (w < 1) w = 1;
    if (h < 1) h = 1;

    // Set the viewport to be the entire window (you probably want to keep this)
    glViewport(0, 0, w, h);
    updateCamera();
}

/**
 * Called whenever m_camera is modified or the canvas is resized. Sets the current OpenGL projection
 * and modelview matrices to match the values in m_camera.
 */
void Lab5Canvas::updateCamera()
{
    float w = width(), h = height();
    float ratio = 1.0f * w / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_camera->angle, ratio, m_camera->near, m_camera->far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(m_camera->eye.x, m_camera->eye.y, m_camera->eye.z,
              m_camera->center.x, m_camera->center.y, m_camera->center.z,
              m_camera->up.x, m_camera->up.y, m_camera->up.z);
}

void Lab5Canvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
        m_lastMousePos.x = event->x(); m_lastMousePos.y = event->y();
}

void Lab5Canvas::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton)
    {
        emit _mouseDrag(event);
        m_lastMousePos.x = event->x(); m_lastMousePos.y = event->y();
    }
}

void Lab5Canvas::mouseDragEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::RightButton || event->buttons() & Qt::LeftButton)
    {
        float dx = event->x() - m_lastMousePos.x,
        dy = event->y() - m_lastMousePos.y;

        double x = m_camera->eye.x, y = m_camera->eye.y, z = m_camera->eye.z,
        r = sqrt(m_camera->eye.x * m_camera->eye.x +
                 m_camera->eye.y * m_camera->eye.y +
                 m_camera->eye.z * m_camera->eye.z),
        theta = acos(y / r) - dy * 0.01f,
        phi = atan2(z, x) + dx * 0.01f;

        if (theta > M_PI-.1) theta = M_PI - .1;
        if (theta < .1) theta = .1;

        m_camera->eye.x = r * sin(theta) * cos(phi);
        m_camera->eye.y = r * cos(theta);
        m_camera->eye.z = r * sin(theta) * sin(phi);

        updateCamera();
    }
}

void Lab5Canvas::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Q)
        exit(0);
}

void Lab5Canvas::redraw()
{
    repaint();
}

void Lab5Canvas::settingsChanged() { }
