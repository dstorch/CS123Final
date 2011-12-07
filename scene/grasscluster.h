#ifndef GRASSCLUSTER_H
#define GRASSCLUSTER_H

#include <QGLWidget>
#include <math.h>
#include <vector.h>

class GrassCluster
{
public:
    GrassCluster(Vector3 position);
    virtual ~GrassCluster();

    void draw();

    void scaleBy(Vector3 scale);

private:
    Vector3 m_position;
    Vector3 m_scale;
};

#endif // GRASSCLUSTER_H
