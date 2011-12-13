#ifndef GRASSPATCH_H
#define GRASSPATCH_H

#include "grasscluster.h"
#include "HeightMap.h"
#include <vector>

class GrassPatch
{
public:
    GrassPatch(int cornerX, int cornerZ, HeightMap *heightmap);
    virtual ~GrassPatch();

    void generateRandomClusters(int numClusters);

    void draw(int texID);
    float getX();
    float getZ();

    void clearPatch();

    vector<GrassCluster>& getClusters();

private:
    Vector2 randomXZ();

    vector<GrassCluster> m_clusters;

    HeightMap* m_heightmap;
    int m_cornerX;
    int m_cornerZ;
};

#endif // GRASSPATCH_H
