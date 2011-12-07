#define HILLS 1.0
#define RADII 5.0
#define SIGMA2X 5.0
#define SIGMA2Y 5.0
#define A 1.0

#include "HeightMap.h"
#include <stdlib.h>
#include "math.h"
#include "stdio.h"
#include "iostream"
#include "boost/random.hpp"

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

    for(int i=0; i<m_rows; i++)
    {
        for( int j=0; j<m_cols; j++)
        {
            m_map[i][j] = 0.0;
        }
    }
}

HeightMap::~HeightMap()
{
    // TODO: cleanup the newed m_map array
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

