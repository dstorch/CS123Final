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

    m_map = new HeightMap(150, 150);
    m_map->generateMap();
    m_map->computeNormals();

    m_camera.eye = Vector3(0.f, 10.f, 0.f);
    m_camera.up = Vector3(0.f, 1.f, 0.f);
    m_camera.theta = M_PI * 1.5f, m_camera.phi = 0.2f;
    m_camera.fovy = 60.f;
    m_camera.heightmap = m_map;

    m_field = GrassField(m_map);
    m_field.makeField();

    m_timeCounter = 0.0;

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
    glmDelete(m_dragon.model);
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

    m_dragon = ResourceLoader::loadObjModel("models/xyzrgb_dragon.obj");
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

    // TODO
    /*m_shaderPrograms["fog"] = ResourceLoader::newShaderProgram(ctx, "shaders/fog.vert", "shaders/fog.frag");
    m_shaderPrograms["depth"] = ResourceLoader::newShaderProgram(ctx, "shaders/depth.vert", "shaders/depth.frag");*/
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

QVector4D GLWidget::windowToFilm(int x, int y, int width, int height)
{
    float xfilm = ((2 * x) / (float) width) - 1.0;
    float yfilm = 1.0 - ((2 * y) / (float) height);
    return QVector4D(xfilm, yfilm, -1.0, 1.0);
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
    m_fps = 1000.f / (time - m_prevTime);
    m_prevTime = time;
    int width = this->width();
    int height = this->height();

    // Render the scene to a framebuffer
    m_framebufferObjects["fbo_0"]->bind();
    applyPerspectiveCamera(width, height);
    renderScene();
    m_framebufferObjects["fbo_0"]->release();

    // TODO: Add drawing code here
    applyOrthogonalCamera(width, height);

    glBindTexture(GL_TEXTURE_2D, m_framebufferObjects["fbo_0"]->texture());
    renderTexturedQuad(width, height, true);
    glBindTexture(GL_TEXTURE_2D, 0);

    paintText();
}

/**
  Renders the scene.  May be called multiple times by paintGL() if necessary.
**/
void GLWidget::renderScene() {
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

    // draw terrain
    m_map->draw(m_soilTex);

    // draw grass on top of terrain
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_grassTex);
    glActiveTexture(GL_TEXTURE0);

    m_timeCounter -= 0.15;
    if (m_timeCounter <= 0.0)
    {
        m_timeCounter = 100.0;
    }

    m_shaderPrograms["grass"]->bind();
    m_shaderPrograms["grass"]->setUniformValue("grassTexture", GL_TEXTURE0);
    m_shaderPrograms["grass"]->setUniformValue("curTime", (GLfloat) m_timeCounter);

    m_field.draw(m_grassTex);

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
        break;
    default:
        break;
    }
}

/**
  Called when the mouse wheel is turned.  Zooms the camera in and out.
**/
void GLWidget::wheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Vertical)
    {
        m_camera.moveForward(event->delta() / 80.0);
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
    case Qt::Key_W:
        m_camera.moveForward(1.5);
        break;
    case Qt::Key_S:
        m_camera.moveForward(-1.5);
        break;
    case Qt::Key_A:
        m_camera.moveRight(-1.5);
        break;
    case Qt::Key_D:
        m_camera.moveRight(1.5);
        break;
    case Qt::Key_Up:
        m_camera.mouseMove(Vector2(0.0, -3.0));
        break;
    case Qt::Key_Down:
        m_camera.mouseMove(Vector2(0.0, 3.0));
        break;
    case Qt::Key_Left:
        m_camera.mouseMove(Vector2(-3.0, 0.0));
        break;
    case Qt::Key_Right:
        m_camera.mouseMove(Vector2(3.0, 0.0));
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
    glColor3f(1.f, 1.f, 1.f);

    // Combine the previous and current framerate
    if (m_fps >= 0 && m_fps < 1000)
    {
        m_prevFps *= 0.95f;
        m_prevFps += m_fps * 0.05f;
    }

    // QGLWidget's renderText takes xy coordinates, a string, and a font
    renderText(10, 20, "FPS: " + QString::number((int) (m_prevFps)), m_font);
    renderText(10, 35, "WASD keys or scroll wheel: move camera", m_font);
    renderText(10, 50, "Arrow keys or mouse: look around", m_font);
}
