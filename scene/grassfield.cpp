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

GrassField::~GrassField()
{
    for (vector<GrassPatch*>::iterator it = m_patches.begin(); it != m_patches.end(); ++it)
    {
        delete *it;
    }
}

void GrassField::clearField()
{
    for (vector<GrassPatch*>::iterator it = m_patches.begin(); it != m_patches.end(); ++it)
    {
        (*it)->clearPatch();
    }
}

void GrassField::denser()
{
    if (constants.clustersPerPatch < 6)
    {
        constants.clustersPerPatch += 1;
        clearField();
        makeField();
    }
}

void GrassField::lessDense()
{
    if (constants.clustersPerPatch > 1)
    {
        constants.clustersPerPatch -= 1;
        clearField();
        makeField();
    }
}

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
