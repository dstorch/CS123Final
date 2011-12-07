#define HILLS 50.0
#define RADII 1000.0
#define SIGMA2X 50.0
#define SIGMA2Y 50.0
#define A 4.0

#include "HeightMap.h"
#include <stdlib.h>
#include "math.h"
#include "stdio.h"
#include "iostream"

using namespace std;


HeightMap::HeightMap(int rows, int cols)
{
    m_rows = rows;
    m_cols = cols;

    m_map = new GLfloat*[rows];
    for( int i=0; i<cols; i++)
    {
        m_map[i] = new GLfloat[cols];
    }

    m_normalMap = new Vector3*[rows*cols];
    for(int i=0; i<m_rows; i++)
    {
        for( int j=0; j<m_cols; j++)
        {
            m_map[i][j] = 0.0;
            m_normalMap[i*m_cols + j] = new Vector3(0.0, 0.0, 0.0);
        }
    }
}

HeightMap::~HeightMap()
{
    // TODO: cleanup the newed m_map array, a 1D vector of GLfloat*
    // TODO: cleanup the newed m_normalMap array, a 1D array of Vector3*
}

GLfloat** HeightMap::generateMap()
{
    srand ( time(NULL) );

    for(int i = 0; i< HILLS; i++)
    {
        int x = floor( rand()%10 / 10.0 * m_rows );
        int z = floor( rand()%10 / 10.0 * m_cols );

        cout << "created hill at : " << x << " , " << z << endl;

        addHill(x,z);
    }
    return m_map;
}

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
                float xContrib = pow( xOff, 2) / 2 / SIGMA2X;
                float zContrib = pow( zOff, 2) / 2 / SIGMA2Y;
                float primary = exp( -(xContrib + zContrib));
                float height = A*primary;
                m_map[row][col] += height;
            }
        }
    }
}

void HeightMap::computeNormals()
{
    // For each vertex in the 2D grid...
    for (int row = 0; row < m_rows; row++)
    {
        for (int column = 0; column < m_cols; column++)
        {
            const GridPoint gridPosition(row, column);                // 2D coordinate of the vertex on the terrain grid
            const int terrainIndex = getIndex(gridPosition);          // Index into the 1D position and normal arrays
            const Vector3& vertexPosition  = Vector3( column, m_map[row][column], row); // Position of the vertex

            // Get the neighbors of the vertex at (a,b)
            const QList<Vector3*>& neighbors = getSurroundingVertices(gridPosition);
            int numNeighbors = neighbors.size();

            // @TODO: [Lab 5] Compute a list of vectors from vertexPosition to each neighbor in neighbors
            Vector3 *offsets = new Vector3[numNeighbors];
            for (int i = 0; i < numNeighbors; ++i)
            {
                offsets[i] = Vector3(0.0, 0.0, 0.0);
                offsets[i] = *neighbors.at(i)- (vertexPosition);
            }

            // @TODO: [Lab 5] Compute cross products for each neighbor
            Vector3 *normals = new Vector3[numNeighbors];
            for (int i = 0; i < numNeighbors; ++i)
            {
                normals[i] = Vector3(0.0, 0.0, 0.0);
                normals[i] = offsets[i].cross(offsets[(i+1) % numNeighbors]);
            }


            // Average the normals and store the final value in the normal map
            Vector3 sum = Vector3(0.0, 0.0, 0.0);
            for (int i = 0; i < numNeighbors; ++i)
                sum += normals[i];
            sum.normalize();
            m_normalMap[terrainIndex]->x = sum.x;
            m_normalMap[terrainIndex]->y = sum.y;
            m_normalMap[terrainIndex]->z = sum.z;

            delete[] offsets;
            delete[] normals;
        }
    }
}

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

void HeightMap::draw()
{
    glMatrixMode(GL_MODELVIEW);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Push a new matrix onto the stack for modelling transformations
    glPushMatrix();

    // @TODO [Lab5] Tesselate your terrain here.

    for(int i = 0; i< m_rows - 1; i++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for(int j=0; j<m_cols; j++)
        {
            glNormal3fv( m_normalMap[getIndex(i,j)]->xyz );
//            glTexCoord2f((float)i/(float)m_gridLength, (float)j/(float)m_gridLength);
            glVertex3fv(Vector3(j - m_rows/2, m_map[i][j], i - m_cols/2).xyz); // NoteToSelf : added a centering offset
//            glTexCoord2f((float)(i+1)/(float)m_gridLength, (float)j/(float)m_gridLength);
            glVertex3fv(Vector3(j - m_rows/2, m_map[i+1][j], i+1 - m_cols/2).xyz); // NoteToSelf : added a centering offset
        }
        glEnd();
    }

    //draw normals
//    drawNormals();
    // Discard the modelling transformations (leaving only camera settings)
    glPopMatrix();
    // Force OpenGL to perform all pending operations -- usually a good idea to call this
//    glFlush();
//    // Swap the buffers to show what we have just drawn onto the screen
//    swapBuffers();
}

//void HeightMap::drawNormals()
//{
//    if (m_renderNormals)
//    {
//        glColor3f(1,0,0);
//
//        for (int row = 0; row < m_rows; row++)
//        {
//            for (int column = 0; column < m_cols; column++)
//            {
//                glBegin(GL_LINES);
//
//                Vector3 curVert = m_terrain[getIndex(row, column)];
//                Vector3 curNorm = m_normalMap[getIndex(row, column)];
//
//                glNormal3dv(curNorm.data);
//                glVertex3f(curVert.x, curVert.y, curVert.z);
//                glVertex3f(curVert.x +curNorm.x,
//                           curVert.y + curNorm.y,
//                           curVert.z + curNorm.z);
//
//                glEnd();
//            }
//        }
//    }
//}
