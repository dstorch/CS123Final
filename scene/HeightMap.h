#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

//#include "glm.h"
#include <GL/gl.h>

class HeightMap
{
public:
    HeightMap(int rows, int cols);
    ~HeightMap();

    GLfloat** generateMap();
    void addHill(int x, int z);
    void printMap();
    float getRand();

private:
    GLfloat** m_map;
    int m_rows;
    int m_cols;
};

#endif // HEIGHTMAP_H
