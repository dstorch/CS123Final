/*!
   @file   glwidget.cpp
   @author dstorch@cs.brown.edu
   @author sl90@cs.brown.edu
   @author zwilson@cs.brown.edu
   @date   December 2011
*/

#include "glwidget.h"

#include <iostream>
#include <QFileDialog>
#include <QGLFramebufferObject>
#include <QGLShaderProgram>
#include <QMouseEvent>
#include <QTime>
#include <QTimer>
#include <QWheelEvent>
#include "glm.h"
#include "Constants.h"

using std::cout;
using std::endl;

extern "C"
{
    extern void APIENTRY glActiveTexture(GLenum);
}

static const int MAX_FPS = 120;

/**
  Constructor.  Initialize all member variables here.
 **/
GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent),
m_timer(this), m_prevTime(0), m_prevFps(0.f), m_fps(0.f),
m_font("Deja Vu Sans Mono", 8, 4)
{

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    constants.initializeConstants();

    m_map = new HeightMap(TERRAIN_HEIGHT, TERRAIN_WIDTH);
    m_map->generateMap();

    m_camera.eye = Vector3(0.f, 10.f, 0.f);
    m_camera.up = Vector3(0.f, 1.f, 0.f);
    m_camera.theta = M_PI * 1.5f, m_camera.phi = 0.2f;
    m_camera.fovy = 60.f;
    m_camera.heightmap = m_map;

    m_field = new GrassField(m_map);
    m_field->makeField();

    m_camera.keepAboveTerrain();

    m_timeCounter = 0.0;
    m_windTime = 0.0;

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
}

/**
  Destructor.  Delete any 'new'ed objects here.
 **/
GLWidget::~GLWidget()
{
    foreach (QGLShaderProgram *sp, m_shaderPrograms)
        delete sp;
    foreach (QGLFramebufferObject *fbo, m_framebufferObjects)
        delete fbo;
    glDeleteLists(m_skybox, 1);
    const_cast<QGLContext *>(context())->deleteTexture(m_cubeMap);
    glmDelete(m_cow.model);

    const_cast<QGLContext *>(context())->deleteTexture(m_grassTex);
    const_cast<QGLContext *>(context())->deleteTexture(m_soilTex);

    delete m_map;
    delete m_field;
}

/**
  Initialize the OpenGL state and start the drawing loop.
 **/
void GLWidget::initializeGL()
{
    // Set up OpenGL
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glDisable(GL_DITHER);

    glDisable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Load resources, including creating shader programs and framebuffer objects
    initializeResources();

    // Start the drawing timer
    m_timer.start(1000.0f / MAX_FPS);
}

/**
  Initialize all resources.
  This includes models, textures, call lists, shader programs, and framebuffer objects.
 **/
void GLWidget::initializeResources()
{
    cout << "Using OpenGL Version " << glGetString(GL_VERSION) << endl << endl;
    // Ideally we would now check to make sure all the OGL functions we use are supported
    // by the video card.  But that's a pain to do so we're not going to.
    cout << "--- Loading Resources ---" << endl;

    m_grassTex = ResourceLoader::loadTexture(QString("textures/grass2.jpg"));
    m_soilTex = ResourceLoader::loadTexture(QString("textures/soil.jpg"));

    m_cow = ResourceLoader::loadObjModel("models/cow.obj");
    cout << "Loaded dragon..." << endl;

    loadCubeMap();
    cout << "Loaded cube map..." << endl;

    createShaderPrograms();
    cout << "Loaded shader programs..." << endl;

    createFramebufferObjects(width(), height());
    cout << "Loaded framebuffer objects..." << endl;

    cout << " --- Finish Loading Resources ---" << endl;
}

/**
  Load a cube map for the skybox
 **/
void GLWidget::loadCubeMap()
{
    QList<QFile *> fileList;
    fileList.append(new QFile("textures/sky/right.jpg"));
    fileList.append(new QFile("textures/sky/left.jpg"));
    fileList.append(new QFile("textures/sky/top.jpg"));
    fileList.append(new QFile("textures/sky/top.jpg"));
    fileList.append(new QFile("textures/sky/front.jpg"));
    fileList.append(new QFile("textures/sky/back.jpg"));
    m_cubeMap = ResourceLoader::loadCubeMap(fileList);

    for (QList<QFile*>::iterator it = fileList.begin(); it != fileList.end(); ++it)
    {
        delete *it;
    }
}

/**
  Create shader programs.
 **/
void GLWidget::createShaderPrograms()
{
    const QGLContext *ctx = context();
    m_shaderPrograms["brightpass"] = ResourceLoader::newFragShaderProgram(ctx, "shaders/brightpass.frag");
    m_shaderPrograms["blur"] = ResourceLoader::newFragShaderProgram(ctx, "shaders/blur.frag");

    m_shaderPrograms["grass"] = ResourceLoader::newShaderProgram(ctx, "shaders/grass.vert", "shaders/grass.frag");

    m_shaderPrograms["fog"] = ResourceLoader::newShaderProgram(ctx, "shaders/fog.vert", "shaders/fog.frag");

    m_shaderPrograms["terrain"] = ResourceLoader::newShaderProgram(ctx, "shaders/terrain.vert", "shaders/terrain.frag");
}

/**
  Allocate framebuffer objects.

  @param width: the viewport width
  @param height: the viewport height
 **/
void GLWidget::createFramebufferObjects(int width, int height)
{
    // Allocate the main framebuffer object for rendering the scene to
    // This needs a depth attachment
    m_framebufferObjects["fbo_0"] = new QGLFramebufferObject(width, height, QGLFramebufferObject::Depth,
                                                             GL_TEXTURE_2D, GL_RGB16F_ARB);
    m_framebufferObjects["fbo_0"]->format().setSamples(16);
    // Allocate the secondary framebuffer obejcts for rendering textures to (post process effects)
    // These do not require depth attachments
    m_framebufferObjects["fbo_1"] = new QGLFramebufferObject(width, height, QGLFramebufferObject::Depth,
                                                             GL_TEXTURE_2D, GL_RGB16F_ARB);
    // TODO: Create another framebuffer here.  Look up two lines to see how to do this... =.=
    m_framebufferObjects["fbo_2"] = new QGLFramebufferObject(width, height, QGLFramebufferObject::NoAttachment,
                                                             GL_TEXTURE_2D, GL_RGB16F_ARB);
}

/**
  Called to switch to an orthogonal OpenGL camera.
  Useful for rending a textured quad across the whole screen.

  @param width: the viewport width
  @param height: the viewport height
**/
void GLWidget::applyOrthogonalCamera(float width, float height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0.f, -1.f, 1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
  Called to switch to a perspective OpenGL camera.

  @param width: the viewport width
  @param height: the viewport height
**/
void GLWidget::applyPerspectiveCamera(float width, float height)
{
    float ratio = ((float) width) / height;
    Vector3 dir(-Vector3::fromAngles(m_camera.theta, m_camera.phi));
    Vector3 center = m_camera.eye;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_camera.fovy, ratio, 0.1f, 1000.f);
    gluLookAt(center.x, center.y, center.z, center.x + dir.x, center.y + dir.y, center.z + dir.z,
              m_camera.up.x, m_camera.up.y, m_camera.up.z);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
  Draws the scene to a buffer which is rendered to the screen when this function exits.
 **/
void GLWidget::paintGL()
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update the fps
    int time = m_clock.elapsed();
    int deltaTime = time - m_prevTime;
    m_fps = 1000.f / deltaTime;
    m_prevTime = time;
    int width = this->width();
    int height = this->height();


    // Render the scene to a framebuffer
    m_framebufferObjects["fbo_0"]->bind();
    applyPerspectiveCamera(width, height);
    renderScene(deltaTime);
    m_framebufferObjects["fbo_0"]->release();

    // Bloom lighting:
    // Copy the rendered scene into framebuffer 1
    m_framebufferObjects["fbo_0"]->blitFramebuffer(m_framebufferObjects["fbo_1"],
                                                   QRect(0, 0, width, height), m_framebufferObjects["fbo_0"],
                                                   QRect(0, 0, width, height), GL_COLOR_BUFFER_BIT, GL_NEAREST);

    applyOrthogonalCamera(width, height);

    glBindTexture(GL_TEXTURE_2D, m_framebufferObjects["fbo_1"]->texture());
    renderTexturedQuad(width, height, true);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_framebufferObjects["fbo_2"]->bind();
    m_shaderPrograms["brightpass"]->bind();
    glBindTexture(GL_TEXTURE_2D, m_framebufferObjects["fbo_1"]->texture());
    renderTexturedQuad(width, height, true);
    m_shaderPrograms["brightpass"]->release();
    glBindTexture(GL_TEXTURE_2D, 0);
    m_framebufferObjects["fbo_2"]->release();

    float scales[] = {4.f,8.f,16.f,32.f,64.f};
    for (int i = 0; i < 4; ++i)
    {
        // Render the blurred brightpass filter result to fbo 1
        renderBlur(width / scales[i], height / scales[i]);

        // Bind the image from fbo to a texture
        glBindTexture(GL_TEXTURE_2D, m_framebufferObjects["fbo_1"]->texture());
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // Enable alpha blending and render the texture to the screen
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glTranslatef(0.f, (scales[i] - 1) * -height, 0.f);
        renderTexturedQuad(width * scales[i], height * scales[i], false);
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    paintText();
}


/**
  Renders the scene.  May be called multiple times by paintGL() if necessary.
**/
void GLWidget::renderScene(int deltaTime) {
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Enable cube maps and draw the skybox
    glEnable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);
    renderSkybox(m_camera.eye);

    // Enable culling (back) faces for rendering the dragon
    glEnable(GL_CULL_FACE);

    glDisable(GL_TEXTURE_CUBE_MAP);

    // cow!
    /*glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.f, 20.0, 0.f);
    glCallList(m_cow.idx);
    glPopMatrix();*/

    QVector3D eye = QVector3D(m_camera.eye.x, m_camera.eye.y, m_camera.eye.z);

    // draw terrain
    glBindTexture(GL_TEXTURE_2D, m_soilTex);
    glActiveTexture(GL_TEXTURE0);

    m_shaderPrograms["terrain"]->bind();

    m_shaderPrograms["terrain"]->setUniformValue("groundTexture", GL_TEXTURE0);
    m_shaderPrograms["terrain"]->setUniformValue("eye", eye);

    m_map->draw(m_soilTex);

    m_shaderPrograms["terrain"]->release();

    glBindTexture(GL_TEXTURE_2D, 0);

    // draw grass on top of terrain
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_grassTex);
    glActiveTexture(GL_TEXTURE0);

    m_timeCounter -= SWAY_SPEED * (float) deltaTime;
    if (m_timeCounter <= 0.0)
    {
        m_timeCounter = INITIAL_TIME;
    }

    m_windTime -= TIME_ATT * (float) deltaTime;
    if (m_windTime <= 0.0)
    {
        m_windTime = 0.0;
    }

    m_shaderPrograms["grass"]->bind();
    m_shaderPrograms["grass"]->setUniformValue("grassTexture", GL_TEXTURE0);

    m_shaderPrograms["grass"]->setUniformValue("curTime", (GLfloat) m_timeCounter);
    m_shaderPrograms["grass"]->setUniformValue("windOrigin", m_windOrigin);
    m_shaderPrograms["grass"]->setUniformValue("windDir", m_windDir);
    m_shaderPrograms["grass"]->setUniformValue("eye", eye);
    m_shaderPrograms["grass"]->setUniformValue("windTime", (GLfloat) m_windTime);

    m_shaderPrograms["grass"]->setUniformValue("ambientWaveAmplitude", (GLfloat) constants.ambientWaveAmplitude);
    m_shaderPrograms["grass"]->setUniformValue("perturbationWaveAmplitude", (GLfloat) constants.perturbationWaveAmplitude);


    m_field->draw(m_grassTex, m_camera.eye);

    m_shaderPrograms["grass"]->release();

    glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);
    glDisable(GL_BLEND);

    // Disable culling, depth testing and cube maps
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    glBindTexture(GL_TEXTURE_2D,0);
    glDisable(GL_TEXTURE_CUBE_MAP);
}

/*!
 * Draw the skybox, always keeping it centered
 * around the camera.
 *
 * @param eye - the current eye position of the camera
 */
void GLWidget::renderSkybox(Vector3 eye)
{
    glBegin(GL_QUADS);
    float extent = 100.f;
    glTexCoord3f( 1.0f, -1.0f, -1.0f); glVertex3f( extent + eye.x, -extent, -extent + eye.z);
    glTexCoord3f(-1.0f, -1.0f, -1.0f); glVertex3f(-extent + eye.x, -extent, -extent + eye.z);
    glTexCoord3f(-1.0f,  1.0f, -1.0f); glVertex3f(-extent + eye.x,  extent, -extent + eye.z);
    glTexCoord3f( 1.0f,  1.0f, -1.0f); glVertex3f( extent + eye.x,  extent, -extent + eye.z);
    glTexCoord3f( 1.0f, -1.0f,  1.0f); glVertex3f( extent + eye.x, -extent,  extent + eye.z);
    glTexCoord3f( 1.0f, -1.0f, -1.0f); glVertex3f( extent + eye.x, -extent, -extent + eye.z);
    glTexCoord3f( 1.0f,  1.0f, -1.0f); glVertex3f( extent + eye.x,  extent, -extent + eye.z);
    glTexCoord3f( 1.0f,  1.0f,  1.0f); glVertex3f( extent + eye.x,  extent,  extent + eye.z);
    glTexCoord3f(-1.0f, -1.0f,  1.0f); glVertex3f(-extent + eye.x, -extent,  extent + eye.z);
    glTexCoord3f( 1.0f, -1.0f,  1.0f); glVertex3f( extent + eye.x, -extent,  extent + eye.z);
    glTexCoord3f( 1.0f,  1.0f,  1.0f); glVertex3f( extent + eye.x,  extent,  extent + eye.z);
    glTexCoord3f(-1.0f,  1.0f,  1.0f); glVertex3f(-extent + eye.x,  extent,  extent + eye.z);
    glTexCoord3f(-1.0f, -1.0f, -1.0f); glVertex3f(-extent + eye.x, -extent, -extent + eye.z);
    glTexCoord3f(-1.0f, -1.0f,  1.0f); glVertex3f(-extent + eye.x, -extent,  extent + eye.z);
    glTexCoord3f(-1.0f,  1.0f,  1.0f); glVertex3f(-extent + eye.x,  extent,  extent + eye.z);
    glTexCoord3f(-1.0f,  1.0f, -1.0f); glVertex3f(-extent + eye.x,  extent, -extent + eye.z);
    glTexCoord3f(-1.0f,  1.0f, -1.0f); glVertex3f(-extent + eye.x,  extent, -extent + eye.z);
    glTexCoord3f(-1.0f,  1.0f,  1.0f); glVertex3f(-extent + eye.x,  extent,  extent + eye.z);
    glTexCoord3f( 1.0f,  1.0f,  1.0f); glVertex3f( extent + eye.x,  extent,  extent + eye.z);
    glTexCoord3f( 1.0f,  1.0f, -1.0f); glVertex3f( extent + eye.x,  extent, -extent + eye.z);
    glTexCoord3f(-1.0f, -1.0f, -1.0f); glVertex3f(-extent + eye.x, -extent, -extent + eye.z);
    glTexCoord3f(-1.0f, -1.0f,  1.0f); glVertex3f(-extent + eye.x, -extent,  extent + eye.z);
    glTexCoord3f( 1.0f, -1.0f,  1.0f); glVertex3f( extent + eye.x, -extent,  extent + eye.z);
    glTexCoord3f( 1.0f, -1.0f, -1.0f); glVertex3f( extent + eye.x, -extent, -extent + eye.z);
    glEnd();
}

/**
  Run a gaussian blur on the texture stored in fbo 2 and
  put the result in fbo 1.  The blur should have a radius of 2.

  @param width: the viewport width
  @param height: the viewport height
**/
void GLWidget::renderBlur(int width, int height)
{
    int radius = 2;
    int dim = radius * 2 + 1;
    GLfloat kernel[dim * dim];
    GLfloat offsets[dim * dim * 2];
    createBlurKernel(radius, width, height, &kernel[0], &offsets[0]);
    // TODO: Finish filling this in

    m_framebufferObjects["fbo_1"]->bind();
    m_shaderPrograms["blur"]->bind();
    glBindTexture(GL_TEXTURE_2D, m_framebufferObjects["fbo_2"]->texture());

    m_shaderPrograms["blur"]->setUniformValueArray(&"offsets"[0], &offsets[0], dim * dim, 2);
    m_shaderPrograms["blur"]->setUniformValueArray(&"kernel"[0], &kernel[0], dim * dim, 1);

    renderTexturedQuad(width, height, false);
    m_shaderPrograms["blur"]->release();
    glBindTexture(GL_TEXTURE_2D, 0);
    m_framebufferObjects["fbo_1"]->release();

}

/**
  Called when the mouse is dragged.  Rotates the camera based on mouse movement.
**/
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    Vector2 pos(event->x(), event->y());
    if (event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton)
    {
        m_camera.mouseMove(pos - m_prevMousePos);
    }
    m_prevMousePos = pos;
}

/**
  Record a mouse press position.
 **/
void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_prevMousePos.x = event->x();
    m_prevMousePos.y = event->y();

    // spawn wind
    switch(event->button())
    {
    case Qt::RightButton:
        spawnWind(event->x(), event->y());
        break;
    default:
        break;
    }
}

/*!
 * Compute the world space point from which the wind originates
 * and the direction of the wind. Wind direction is based on the
 * current look vector.
 *
 * @param xclick - the x-coordinate of the mouse click
 * @param yclick - the y-coordinate of the mouse click
 */
void GLWidget::spawnWind(int xclick, int yclick)
{
    m_windTime = INITIAL_TIME;

    int xcenter = this->width() / 2;
    int ycenter = this->height() / 2;
    float ratio = ((float) this->width()) / this->height();

    float halfy = m_camera.fovy / 2.0;

    float dx = (float) (xclick - xcenter);
    float dy = (float) (yclick - ycenter);

    float halfx = (m_camera.fovy * ratio) / 2.0;

    halfx *= (M_PI / 180.0);
    halfy *= (M_PI / 180.0);

    float deltaPhi = atan(((float) dy / (float) ycenter) * tan(halfy));
    float deltaTheta = atan(((float) dx / (float) xcenter) * tan(halfx));

    float phi = m_camera.phi + deltaPhi;
    float theta = m_camera.theta + deltaTheta;

    Vector3 windLook(-Vector3::fromAngles(theta, phi));

    float t = -1.0 * (m_camera.eye.y / windLook.y);

    Vector3 worldPoint = m_camera.eye + t * windLook;

    m_windOrigin = QVector4D(worldPoint.x, worldPoint.y, worldPoint.z, 1.0);

    Vector3 dir(-Vector3::fromAngles(m_camera.theta, m_camera.phi));
    m_windDir = QVector4D(dir.x, dir.y, dir.z, 0.0);
}

/**
  Called when the mouse wheel is turned.  Zooms the camera in and out.
**/
void GLWidget::wheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Vertical)
    {
        m_camera.moveForward(event->delta() / CAM_WHEEL_SENSITIVITY);
    }
}

/**
  Called when the GLWidget is resized.
 **/
void GLWidget::resizeGL(int width, int height)
{
    // Resize the viewport
    glViewport(0, 0, width, height);

    // Reallocate the framebuffers with the new window dimensions
    foreach (QGLFramebufferObject *fbo, m_framebufferObjects)
    {
        const QString &key = m_framebufferObjects.key(fbo);
        QGLFramebufferObjectFormat format = fbo->format();
        delete fbo;
        m_framebufferObjects[key] = new QGLFramebufferObject(width, height, format);
    }
}

/**
  Draws a textured quad. The texture most be bound and unbound
  before and after calling this method - this method assumes that the texture
  has been bound before hand.

  @param w: the width of the quad to draw
  @param h: the height of the quad to draw
  @param flip: flip the texture vertically
**/
void GLWidget::renderTexturedQuad(int width, int height, bool flip) {
    // Clamp value to edge of texture when texture index is out of bounds
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Draw the  quad
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, flip ? 1.0f : 0.0f);
    glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, flip ? 1.0f : 0.0f);
    glVertex2f(width, 0.0f);
    glTexCoord2f(1.0f, flip ? 0.0f : 1.0f);
    glVertex2f(width, height);
    glTexCoord2f(0.0f, flip ? 0.0f : 1.0f);
    glVertex2f(0.0f, height);
    glEnd();
}

/**
  Creates a gaussian blur kernel with the specified radius.  The kernel values
  and offsets are stored.

  @param radius: The radius of the kernel to create.
  @param width: The width of the image.
  @param height: The height of the image.
  @param kernel: The array to write the kernel values to.
  @param offsets: The array to write the offset values to.
**/
void GLWidget::createBlurKernel(int radius, int width, int height,
                                GLfloat* kernel, GLfloat* offsets)
{
    int size = radius * 2 + 1;
    float sigma = radius / 3.0f;
    float twoSigmaSigma = 2.0f * sigma * sigma;
    float rootSigma = sqrt(twoSigmaSigma * M_PI);
    float total = 0.0f;
    float xOff = 1.0f / width, yOff = 1.0f / height;
    int offsetIndex = 0;
    for (int y = -radius, idx = 0; y <= radius; ++y)
    {
        for (int x = -radius; x <= radius; ++x,++idx)
        {
            float d = x * x + y * y;
            kernel[idx] = exp(-d / twoSigmaSigma) / rootSigma;
            total += kernel[idx];
            offsets[offsetIndex++] = x * xOff;
            offsets[offsetIndex++] = y * yOff;
        }
    }
    for (int i = 0; i < size * size; ++i)
    {
        kernel[i] /= total;
    }
}

/**
  Handles any key press from the keyboard
 **/
void GLWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_U:
        constants.ambientWaveAmplitude = min( constants.ambientWaveAmplitude+.1, 4.0);
        break;
    case Qt::Key_I:
        constants.ambientWaveAmplitude = max( constants.ambientWaveAmplitude - .1, 0.0);
        break;
    case Qt::Key_J:
        constants.perturbationWaveAmplitude = min(constants.perturbationWaveAmplitude + 1.0, 10.0 );
        break;
    case Qt::Key_K:
        constants.perturbationWaveAmplitude = max( constants.perturbationWaveAmplitude - 1.0, 0.0);
        break;
    case Qt::Key_W:
        m_camera.moveForward(CAM_TRANSLATE_SPEED);
        break;
    case Qt::Key_S:
        m_camera.moveForward(-CAM_TRANSLATE_SPEED);
        break;
    case Qt::Key_A:
        m_camera.moveRight(-CAM_TRANSLATE_SPEED);
        break;
    case Qt::Key_D:
        m_camera.moveRight(CAM_TRANSLATE_SPEED);
        break;
    case Qt::Key_Up:
        if (m_map->moreHills())
        {
            m_map->resetMap();
            m_map->generateMap();
            m_field->clearField();
            m_field->makeField();
            m_camera.keepAboveTerrain();
        }
        break;
    case Qt::Key_Down:
        if (m_map->lessHills())
        {
            m_map->resetMap();
            m_map->generateMap();
            m_field->clearField();
            m_field->makeField();
            m_camera.keepAboveTerrain();
        }
        break;
    case Qt::Key_M:
        m_field->denser();
        break;
    case Qt::Key_L:
        m_field->lessDense();
        break;
    case Qt::Key_P:
        QImage qi = grabFrameBuffer(false);
        QString filter;
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("PNG Image (*.png)"), &filter);
        qi.save(QFileInfo(fileName).absoluteDir().absolutePath() + "/" + QFileInfo(fileName).baseName() + ".png", "PNG", 100);
        break;
    }
}

/**
  Draws text for the FPS and screenshot prompt
 **/
void GLWidget::paintText()
{
    glColor3f(0.f, 0.f, 0.f);

    // Combine the previous and current framerate
    if (m_fps >= 0 && m_fps < 1000)
    {
        m_prevFps *= 0.95f;
        m_prevFps += m_fps * 0.05f;
    }

    // QGLWidget's renderText takes xy coordinates, a string, and a font
    renderText(10, 20, "FPS: " + QString::number((int) (m_prevFps)), m_font);
    renderText(10, 35, "M/L: more or less grass", m_font);
    renderText(10, 50, "Up/Down: more or less hilly", m_font);
    renderText(10, 65, "U/I: more or less ambient wave", m_font);
    renderText(10, 80, "J/K: more or less click perturbation", m_font);

    glColor3f(1.f, 1.f, 1.f);
}
