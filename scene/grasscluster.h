/*!
   @file   grasscluster.h
   @author dstorch@cs.brown.edu
   @author sl90@cs.brown.edu
   @author zwilson@cs.brown.edu
   @date   December 2011
*/

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

    void setDistance(Vector3 eye);
    float getDistance();

private:
    Vector3 m_position;
    Vector3 m_scale;
    Vector3 m_up;

    float m_distance;
};

#endif // GRASSCLUSTER_H
