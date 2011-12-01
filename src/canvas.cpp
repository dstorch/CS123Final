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
    //You may want to add code here
    m_renderNormals = false;
    m_camera = new LabCamera();
    m_camera->eye.x = 0.0f, m_camera->eye.y = 20.f, m_camera->eye.z = 10.0f;
    m_camera->center.x = 0.0f, m_camera->center.y = 0.0f, m_camera->center.z = 0.0f;
    m_camera->up.x = 0.0f, m_camera->up.y = 1.0f, m_camera->up.z = 0.0f;
    m_camera->angle = 45.0f, m_camera->near = 1.0f, m_camera->far = 1000.0f;

    // Determines how much each vertex gets perturbed. The larger the value, the less pertubration will occur per recursive value
    m_decay = 2;

    // The number of levels of recursion your terrain uses (can be considered the level of detail of your terrain)
    m_depth = 8;

    // The roughness of your terrain. Higher roughnesses entail taller mountains and deeper valleys. Lower values entail small hills and shallow valleys
    m_roughness = 6;

    // Seed a random number, your terrain will change based on what number you seed
    srand(0);

    // Do not change below here
    QObject::connect(this, SIGNAL(_mouseDrag(QMouseEvent*)), this, SLOT(mouseDragEvent(QMouseEvent*)));
    setAutoBufferSwap(false);
    setFocusPolicy(Qt::StrongFocus);
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(redraw()));
    m_timer->start(1000.0f / m_fps);

    // Imagining your terrain as a square grid of points, gridLength is the number of elements on one side, or the length of a side of the grid
    m_gridLength = (1 << m_depth)+1;

    // Knowing the number of elements on a side gives us the total number of elements in the grid
    int terrain_array_size = m_gridLength * m_gridLength;

    // Represents all of our vertices. Picture this as a grid
    m_terrain = new Vector3[terrain_array_size];

    // Represents all the normals, one per each vertex
    m_normalMap = new Vector3[terrain_array_size];
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

    delete[] m_terrain;
    delete[] m_normalMap;
}

// You will only need to deal with the next 3 (maybe 4) methods

/**
 * You need to fill this in.
 *
 * Computes the normal vector of each terrain vertex and stores it in the corresponding vertex.
 */
void Lab5Canvas::computeNormals()
{

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

    //draw normals
    drawNormals();
    // Discard the modelling transformations (leaving only camera settings)
    glPopMatrix();
    // Force OpenGL to perform all pending operations -- usually a good idea to call this
    glFlush();
    // Swap the buffers to show what we have just drawn onto the screen
    swapBuffers();
}


/****************************************************************************************************************/
/**********************************DO NOT MODIFY ANY OF THE FOLLOWING CODE***************************************/
/****************************************************************************************************************/

/**
 * Draws a line at each vertex showing the direction of that vertex's normal. You may find
 * this to be a useful tool if you're having trouble getting the lighting to look right.
 * By default, this function is called in paintGL(), but only renders anything if
 * m_renderNormals is true. You do not need to modify this function.
 */
void Lab5Canvas::drawNormals()
{

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

    computeNormals();
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

}

void Lab5Canvas::mouseMoveEvent(QMouseEvent *event)
{

}

void Lab5Canvas::mouseDragEvent(QMouseEvent *event)
{

}

void Lab5Canvas::keyPressEvent(QKeyEvent *event)
{

}

void Lab5Canvas::redraw()
{
    repaint();
}

void Lab5Canvas::settingsChanged() { }
