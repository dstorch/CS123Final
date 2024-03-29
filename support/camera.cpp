/*!
   @file   camera.cpp
   @author dstorch@cs.brown.edu
   @author sl90@cs.brown.edu
   @author zwilson@cs.brown.edu
   @date   December 2011
*/

#include "camera.h"
#include <qgl.h>

void OrbitCamera::mouseMove(const Vector2 &delta)
{
    // Rotate the eye vector around the origin
    theta += delta.x * 0.01f;
    phi += delta.y * 0.01f;

    // Keep theta in [0, 2pi] and phi in [-phi/2, pi/2]
    theta -= floorf(theta / M_2PI) * M_2PI;
    phi = max(0.01f - M_PI / 2, min(M_PI / 2 - 0.01f, phi));
}

/*!
 * Move the camera along the look vector.
 *
 * @param amount - the distance to move by,
 *  in world space
 */
void OrbitCamera::moveForward(float amount)
{
    Vector3 dir(-Vector3::fromAngles(theta, phi));
    Vector3 newCenter = eye + amount * dir;

    if (inBoundingBox(newCenter)) eye = newCenter;
}

/*!
 * Move to the right without changing the look
 * direction or the up vector.
 *
 * @param amount - the distance to move by,
 *  in world space
 */
void OrbitCamera::moveRight(float amount)
{
    Vector3 dir(-Vector3::fromAngles(theta, phi));

    Vector3 right = dir.cross(up);
    right.normalize();

    Vector3 newCenter = eye + amount * right;

    if (inBoundingBox(newCenter)) eye = newCenter;
}

/*!
 * Test whether the camera eye position is within
 * the allowed bounding box.
 *
 * @return true if in bounding box, false otherwise
 */
bool OrbitCamera::inBoundingBox(Vector3 point)
{
    bool testx = point.x < 50.0 && point.x > -50.0;
    bool testz = point.z < 50.0 && point.z > -50.0;

    int xcoord = max(0, min(heightmap->height() - 1, point.x + (TERRAIN_HEIGHT / 2.0)));
    int zcoord = max(0, min(heightmap->width() - 1, point.z + (TERRAIN_WIDTH / 2.0)));
    float height = heightmap->getFromHeightMap(zcoord, xcoord);

    bool testy = point.y > (height + 1.0) && point.y < 25.0;

    return testx && testy && testz;
}

/*!
 * After generating a new terrain map, this method
 * should be called. Makes sure that the camera does
 * not get trapped below the terrain.
 */
void OrbitCamera::keepAboveTerrain()
{
    int xcoord = max(0, min(heightmap->height() - 1, eye.x + (TERRAIN_HEIGHT / 2.0)));
    int zcoord = max(0, min(heightmap->width() - 1, eye.z + (TERRAIN_WIDTH / 2.0)));

    float height = heightmap->getFromHeightMap(zcoord, xcoord);

    if (height > (eye.y + 2.0))
    {
        eye.y = height + 4.0;
    }
}
