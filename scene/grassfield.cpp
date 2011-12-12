#include "grassfield.h"

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
        //delete *it;
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
            patch->generateRandomClusters(CLUSTERS_PER_PATCH);
            m_patches.push_back(patch);
        }
    }
}

void GrassField::draw(int texID)
{
    for (vector<GrassPatch*>::iterator it = m_patches.begin(); it != m_patches.end(); ++it)
    {
        (*it)->draw(texID);
    }
}
