#ifndef CAMERA_H
#define CAMERA_H

#include <QMouseEvent>
#include "vector.h"
#include "HeightMap.h"
#include "Constants.h"

/**
    An orbiting perspective camera specified by a center, two angles, and a zoom factor

    @author: Justin Ardini (jardini)
**/
struct OrbitCamera
{
    Vector3 eye, up;
    float theta, phi;
    float fovy;
    HeightMap* heightmap;

    void mouseMove(const Vector2 &delta);

    void moveForward(float amount);
    void moveRight(float amount);
    bool inBoundingBox(Vector3 center);
};

#endif // CAMERA_H
