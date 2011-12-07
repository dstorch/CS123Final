#include "grasscluster.h"

GrassCluster::GrassCluster(Vector3 position)
{
    m_position = position;
    m_scale = Vector3(1.0, 1.0, 1.0);
}

GrassCluster::~GrassCluster()
{
}

void GrassCluster::scaleBy(Vector3 scale)
{
    m_scale = scale;
}

void GrassCluster::draw()
{

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(m_position.x, m_position.y, m_position.z);
    glScalef(m_scale.x, m_scale.y, m_scale.z);

    glBegin(GL_QUADS);

    // front
    //glNormal3f(0.0, 0.0, -1.0);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    glVertex3f(1.0, 3.0, 0.0);
    glVertex3f(-1.0, 3.0, 0.0);
    glVertex3f(-1.0, 0.0, 0.0);

    // back
    //glNormal3f(0.0, 0.0, 1.0);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(-1.0, 0.0, 0.0);
    glVertex3f(-1.0, 3.0, 0.0);
    glVertex3f(1.0, 3.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);

    float theta = M_PI / 3.0;
    float x = cos(theta);
    float z = sin(theta);

    float normalangle = 7.0 * (M_PI / 12.0);
    float xnormal = cos(normalangle);
    float znormal = sin(normalangle);

    // front
    //glNormal3f(xnormal, 0.0, znormal);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(x, 0.0, z);
    glVertex3f(x, 3.0, z);
    glVertex3f(-x, 3.0, -z);
    glVertex3f(-x, 0.0, -z);

    // back
    //glNormal3f(-xnormal, 0.0, -znormal);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(-x, 0.0, -z);
    glVertex3f(-x, 3.0, -z);
    glVertex3f(x, 3.0, z);
    glVertex3f(x, 0.0, z);

    theta = 2.0 * (M_PI / 3.0);
    x = cos(theta);
    z = sin(theta);

    normalangle = 5.0 * (M_PI / 12.0);
    xnormal = cos(normalangle);
    znormal = sin(normalangle);

    // front
    //glNormal3f(xnormal, 0.0, znormal);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(x, 0.0, z);
    glVertex3f(x, 3.0, z);
    glVertex3f(-x, 3.0, -z);
    glVertex3f(-x, 0.0, -z);

    // back
    //glNormal3f(-xnormal, 0.0, -znormal);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(-x, 0.0, -z);
    glVertex3f(-x, 3.0, -z);
    glVertex3f(x, 3.0, z);
    glVertex3f(x, 0.0, z);

    glPopMatrix();

    glEnd();
}
