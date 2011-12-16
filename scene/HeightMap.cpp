/*!
   @file   HeightMap.cpp
   @author dstorch@cs.brown.edu
   @author sl90@cs.brown.edu
   @author zwilson@cs.brown.edu
   @date   December 2011
*/

#include "HeightMap.h"
#include <stdlib.h>
#include "math.h"
#include "stdio.h"
#include "iostream"
#include "Constants.h"

using namespace std;


HeightMap::HeightMap(int rows, int cols)
{
    m_rows = rows;
    m_cols = cols;

    // setup for the heightmap
    m_map = new GLfloat*[cols];
    for( int i=0; i<cols; i++)
    {
        m_map[i] = new GLfloat[rows];
    }

    resetMap();
}

HeightMap::~HeightMap()
{
    for(int i=0; i<m_rows; i++)
    {
        delete[] m_map[i];
    }
    delete[] m_map;
}

/*!
 * Adds more hills to m_map by changing the constants
 * governing the hilliness and spread of the hills
 */
bool HeightMap::moreHills()
{
    if (constants.a < 8.0)
    {
        constants.a += 0.25;
        constants.hills += 2;
        constants.sigma2x += 10.0;
        constants.sigma2y += 10.0;

        return true;
    }

    return false;
}

/*!
 * Does the opposite of moreHills()
 */
bool HeightMap::lessHills()
{
    if (constants.a > 0.25)
    {
        constants.a -= 0.25;
        constants.hills -= 2;
        constants.sigma2x -= 10.0;
        constants.sigma2y -= 10.0;

        return true;
    }

    return false;
}

/*!
 * Resets the height map by setting the maps height values to zero.
 */
void HeightMap::resetMap()
{
    for(int i=0; i<m_rows; i++)
    {
        for( int j=0; j<m_cols; j++)
        {
            m_map[i][j] = 0.0;
        }
    }
}

/*!
 * Randomly find points in the height map and calls
 * addHill on that point (see addHill)
 */
GLfloat** HeightMap::generateMap()
{
    srand ( time(NULL) );

    for(int i = 0; i< constants.hills; i++)
    {
        int x = floor( rand()%10 / 10.0 * m_rows );
        int z = floor( rand()%10 / 10.0 * m_cols );

        addHill(x,z);
    }

    return m_map;
}

/*!
 * Effectively adds a hill to the height map by
 * iterating through the points in the map and
 * checking their distance from the hill point.
 * If the point is within the specified hill's range,
 * then a height value is calculated for the point and
 * added to the corresponding element in the height map.
 *
 * @param int x, int z
 * the hill point
 */
void HeightMap::addHill(int x, int z)
{
    int diameter = RADII * 2;
    int numPoints = pow( diameter, 2);

    int startRow = max(0.0, z-RADII);
    int endRow = min((double)m_cols, z+RADII);

    int startCol = max(0.0, x-RADII);
    int endCol = min((double)m_cols, x+RADII);

    for(int row = startRow ; row < endRow; row++)
    {
        for(int col = startCol; col<endCol; col++)
        {
            float xOff = col-x;
            float zOff = row-z;
            float dist2 = pow(zOff, 2) + pow(xOff, 2);
            if( dist2 > RADII*RADII)
                continue;
            else
            {
                float xContrib = pow( xOff, 2) / 2 / constants.sigma2x;
                float zContrib = pow( zOff, 2) / 2 / constants.sigma2y;
                float primary = exp( -(xContrib + zContrib));
                float height = constants.a*primary;
                m_map[row][col] += height;
            }
        }
    }
}

int HeightMap::width()
{
    return m_cols;
}

int HeightMap::height()
{
    return m_rows;
}

float HeightMap::getFromHeightMap(int row, int col)
{
    return m_map[row][col];
}

/*!
 * Return the coordinates of the 8 vertices surrounding a given point
 *
 * @param - coordinate, the middle point for which the surrounding vertices are to be found
 */
QList<Vector3*> HeightMap::getSurroundingVertices(const GridPoint &coordinate)
{
    GridPoint coords[8];
    coords[0] = GridPoint(coordinate.x,     coordinate.y - 1);
    coords[1] = GridPoint(coordinate.x + 1, coordinate.y - 1);
    coords[2] = GridPoint(coordinate.x + 1, coordinate.y);
    coords[3] = GridPoint(coordinate.x + 1, coordinate.y + 1);
    coords[4] = GridPoint(coordinate.x,     coordinate.y + 1);
    coords[5] = GridPoint(coordinate.x - 1, coordinate.y + 1);
    coords[6] = GridPoint(coordinate.x - 1, coordinate.y);
    coords[7] = GridPoint(coordinate.x - 1, coordinate.y - 1);

    int index;
    QList<Vector3*> vecs;

    for (int i = 0; i < 8; i++)
    {
        index = getIndex(coords[i]);
        if (index != -1)
        {
            int row =floor(index/m_cols);
            int col = (double)(index%m_cols);
            vecs.push_back(new Vector3( col, m_map[row][col], row)); // TODO: make sure this vector3 is deleted later
        }
    }

    return vecs;
}

/*!
 * prints out the height map
 */
void HeightMap::printMap()
{
    cout << "Map Heights : " << endl;
    for(int i=0; i<m_rows; i++)
    {
        for( int j=0; j<m_cols; j++)
        {
            cout << m_map[i][j] << " | ";
        }
        cout << endl;
    }
}

inline int HeightMap::getIndex(const GridPoint &c)
{
    return getIndex(c.x, c.y);
}

inline int HeightMap::getIndex(int row, int col)
{
    if (row < 0 || row >= m_rows || col < 0 || col >= m_cols)
        return -1;

    return row * m_cols + col;
}

/*!
 * Uses GL_TRIANGLE_STRIP to send vertices to GL and draw the terrain
 * Also passes texture coordinates along the way.
 *
 * @param - texID the ID of the terrain's texture
 */
void HeightMap::draw(GLuint texID)
{
    glMatrixMode(GL_MODELVIEW);

    // Push a new matrix onto the stack for modelling transformations
    glPushMatrix();
    glLoadIdentity();

    glBindTexture(GL_TEXTURE_2D, texID);

    for(int i = 0; i< m_rows - 1; i++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for(int j=0; j<m_cols; j++)
        {
            glTexCoord2f((float)i/(float)m_cols, (float)j/(float)m_rows);
            glVertex3fv(Vector3(j - m_rows/2, m_map[i][j], i - m_cols/2).xyz);
            glTexCoord2f((float)(i+1)/(float)m_cols, (float)j/(float)m_rows);
            glVertex3fv(Vector3(j - m_rows/2, m_map[i+1][j], i+1 - m_cols/2).xyz);
        }
        glEnd();
    }

    glPopMatrix();
}
