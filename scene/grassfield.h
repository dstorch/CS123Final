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

    void draw(int texID);

    static const int CLUSTERS_PER_PATCH = 2;

private:
    HeightMap *m_heightmap;

    vector<GrassPatch*> m_patches;
};

#endif // GRASSFIELD_H
