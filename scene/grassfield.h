#ifndef GRASSFIELD_H
#define GRASSFIELD_H

#include "HeightMap.h"
#include "grasspatch.h"
#include <vector>

class GrassField
{
public:
    GrassField(HeightMap *heightmap);
    GrassField();
    virtual ~GrassField();

    void makeField();

    void draw(int texID, Vector3 eye);
//    bool comp(GrassPatch* a, GrassPatch* b);

    static const int CLUSTERS_PER_PATCH = 4;
    static const int PATCH_SIZE = 2;

private:
    HeightMap *m_heightmap;

    vector<GrassPatch*> m_patches;
    Vector3 m_eye;
};

#endif // GRASSFIELD_H
