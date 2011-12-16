/*!
   @file   HeightMap.h
   @author dstorch@cs.brown.edu
   @author sl90@cs.brown.edu
   @author zwilson@cs.brown.edu
   @date   December 2011
*/

#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <GL/gl.h>
#include "vector.h"
#include "qlist.h"
#include "Constants.h"

typedef Vector2 GridPoint;

/**

 @class HeightMap

 @brief Generates "rolling hills"
 height map. Also provides an interface
 for accessing these height values.

**/
class HeightMap
{
public:
    HeightMap(int rows, int cols);
    ~HeightMap();

    GLfloat** generateMap();
    void addHill(int x, int z);

    void resetMap();

    bool moreHills();
    bool lessHills();

    void printMap();
    float getRand();

    int width();
    int height();
    float getFromHeightMap(int row, int col);

    QList<Vector3*> getSurroundingVertices(const GridPoint &coordinate);
    void draw(GLuint texID);

    inline int getIndex(const GridPoint &c);
    inline int getIndex(int row, int column);

private:
    GLfloat** m_map;
    int m_rows;
    int m_cols;
};

#endif // HEIGHTMAP_H
