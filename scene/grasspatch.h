/*!
   @file   grasspatch.h
   @author dstorch@cs.brown.edu
   @author sl90@cs.brown.edu
   @author zwilson@cs.brown.edu
   @date   December 2011
*/

#ifndef GRASSPATCH_H
#define GRASSPATCH_H

#include "grasscluster.h"
#include "HeightMap.h"
#include <vector>
using namespace std;

/**

 @class GrassPatch

 @brief A GrassPatch stores a fixed number
 of GrassCluster objects, each positioned on
 a certain grid square of the height map.
 The clusters within the patch are positioned
 randomly.

**/
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
