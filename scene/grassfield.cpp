/*!
   @file   grassfield.cpp
   @author dstorch@cs.brown.edu
   @author sl90@cs.brown.edu
   @author zwilson@cs.brown.edu
   @date   December 2011
*/

#include "grassfield.h"
#include "vector"
#include <queue>

using namespace std;

GrassField::GrassField(HeightMap *heightmap)
{
    m_heightmap = heightmap;
}

GrassField::GrassField()
{
}

//! destructor
GrassField::~GrassField()
{
    for (vector<GrassPatch*>::iterator it = m_patches.begin(); it != m_patches.end(); ++it)
    {
        delete *it;
    }
}

/*!
 * Delete all clusters in preparation for
 * producing a new field of grass.
 */
void GrassField::clearField()
{
    for (vector<GrassPatch*>::iterator it = m_patches.begin(); it != m_patches.end(); ++it)
    {
        (*it)->clearPatch();
        delete (*it);
    }

    m_patches.clear();
}

/*!
 * Regenerate the field of grass
 * after incrementing the density.
 *
 * Don't let density go above 6.
 */
void GrassField::denser()
{
    if (constants.clustersPerPatch < 6)
    {
        constants.clustersPerPatch += 1;
        clearField();
        makeField();
    }
}

/*!
 * Regenerate the field of grass
 * after decrementing the density.
 *
 * Don't let density drop below 1.
 */
void GrassField::lessDense()
{
    if (constants.clustersPerPatch > 1)
    {
        constants.clustersPerPatch -= 1;
        clearField();
        makeField();
    }
}

/*!
 * Generate the whole field, with randomized
 * positions and cluster heights.
 */
void GrassField::makeField()
{
    srand((unsigned)time(0));

    for (int i = 0; i < m_heightmap->height() - 1; i += PATCH_SIZE)
    {
        for (int j = 0; j < m_heightmap->width() - 1; j += PATCH_SIZE)
        {
            GrassPatch *patch = new GrassPatch(i, j, m_heightmap);
            patch->generateRandomClusters(constants.clustersPerPatch);
            m_patches.push_back(patch);
        }
    }
}

/*!
 * comparator for sorting clusters back to front
 * with respect to the camera
 */
bool clusterDistanceComp(GrassCluster &c1, GrassCluster &c2)
{
    return c1.getDistance() > c2.getDistance();
}

void GrassField::draw(int texID, Vector3 eye)
{
    QList<GrassCluster> clusterList = QList<GrassCluster>();

    for (vector<GrassPatch*>::iterator it = m_patches.begin(); it != m_patches.end(); ++it)
    {
        vector<GrassCluster> clusters = (*it)->getClusters();

        for (vector<GrassCluster>::iterator it = clusters.begin(); it != clusters.end(); ++it)
        {
            GrassCluster cluster = (*it);
            cluster.setDistance(eye);

            clusterList.push_back(cluster);
        }
    }

    qSort(clusterList.begin(), clusterList.end(), clusterDistanceComp);

    for (QList<GrassCluster>::iterator it = clusterList.begin(); it != clusterList.end(); ++it)
    {

        (*it).draw(texID);
    }
}
