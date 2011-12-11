#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <GL/gl.h>
#include "vector.h"
#include "qlist.h"

typedef Vector2 GridPoint;

class HeightMap
{
public:
    HeightMap(int rows, int cols);
    ~HeightMap();

    GLfloat** generateMap();
    void addHill(int x, int z);

    void computeNormals();

    void printMap();
    float getRand();

    int width();
    int height();
    float getFromHeightMap(int row, int col);
    Vector3* getNormal(int row, int col);

    QList<Vector3*> getSurroundingVertices(const GridPoint &coordinate);
    void draw(GLuint texID);
//    void drawNormals();
    inline int getIndex(const GridPoint &c);
    inline int getIndex(int row, int column);

private:
    GLfloat** m_map;
    Vector3** m_normalMap;
    int m_rows;
    int m_cols;
};

#endif // HEIGHTMAP_H
