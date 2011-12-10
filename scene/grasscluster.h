#ifndef GRASSCLUSTER_H
#define GRASSCLUSTER_H

#include <QGLWidget>
#include <math.h>
#include <vector.h>

// TODO
#include <iostream>
using namespace std;

class GrassCluster
{
public:
    GrassCluster(Vector3 position);
    virtual ~GrassCluster();

    void draw(int texID);

    void scaleBy(Vector3 scale);

private:
    Vector3 m_position;
    Vector3 m_scale;
    Vector3 m_up;
};

#endif // GRASSCLUSTER_H
