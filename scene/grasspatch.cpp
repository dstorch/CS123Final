
#include "grasspatch.h"

GrassPatch::GrassPatch(int cornerX, int cornerZ, HeightMap *heightmap)
{
    m_cornerX = cornerX;
    m_cornerZ = cornerZ;
    m_heightmap = heightmap;
}

GrassPatch::~GrassPatch()
{
}

Vector2 GrassPatch::randomXZ()
{
    float rx = ((float)rand()/(float)RAND_MAX);
    float rz = ((float)rand()/(float)RAND_MAX);

    float outx = ((float) m_cornerX) + (rx * 3.0);
    float outz = ((float) m_cornerZ) + (rz * 3.0);

    return Vector2(outx, outz);
}

void GrassPatch::generateRandomClusters(int numClusters)
{
    for (int i = 0; i < numClusters; i++)
    {
        Vector2 coord = randomXZ();

        // weights for bilinear interpolation
        float xweight = coord.x - floor(coord.x);
        float zweight = coord.y - floor(coord.y);
        float w1 = (1.0 - xweight) * (1.0 - zweight);
        float w2 = xweight * (1.0 - zweight);
        float w3 = (1.0 - xweight) * zweight;
        float w4 = xweight * zweight;

        int xlow = max(0, min(m_heightmap->height()-1, floor(coord.x)));
        int xhigh = max(0, min(m_heightmap->height()-1, xlow + 1));
        int zlow = max(0, min(m_heightmap->width()-1, floor(coord.y)));
        int zhigh = max(0, min(m_heightmap->width()-1, zlow + 1));

        float height1 = m_heightmap->getFromHeightMap(xlow, zlow);
        float height2 = m_heightmap->getFromHeightMap(xlow, zhigh);
        float height3 = m_heightmap->getFromHeightMap(xhigh, zlow);
        float height4 = m_heightmap->getFromHeightMap(xhigh, zhigh);

        float ycoord = w1 * height1 + w2 * height2 + w3 * height3 + w4 * height4;
        float zcoord = coord.x - (float) (m_heightmap->height() / 2);
        float xcoord = coord.y - (float) (m_heightmap->width() / 2);

        GrassCluster cluster = GrassCluster(Vector3(xcoord, ycoord, zcoord));

        // scale in y-direction by a random number
        float r = ((float)rand()/((float)RAND_MAX * 2.0)) + 1.0;
        cluster.scaleBy(Vector3(1.5f, r, 1.f));

        m_clusters.push_back(cluster);
    }
}

float GrassPatch::getX()
{
    return m_cornerX;
}

float GrassPatch::getZ()
{
    return m_cornerZ;
}

vector<GrassCluster>& GrassPatch::getClusters()
{
    return m_clusters;
}


void GrassPatch::draw(int texID)
{
    for (vector<GrassCluster>::iterator it = m_clusters.begin(); it != m_clusters.end(); ++it)
    {
        (*it).draw(texID);
    }
}

void GrassPatch::clearPatch()
{
        m_clusters.clear();
}
