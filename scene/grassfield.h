/*!
   @file   grassfield.h
   @author dstorch@cs.brown.edu
   @author sl90@cs.brown.edu
   @author zwilson@cs.brown.edu
   @date   December 2011
*/

#ifndef GRASSFIELD_H
#define GRASSFIELD_H

#include "HeightMap.h"
#include "grasspatch.h"
#include "Constants.h"
#include <vector>

class GrassField
{
public:
    GrassField(HeightMap *heightmap);
    GrassField();
    virtual ~GrassField();

    void makeField();

    void draw(int texID, Vector3 eye);

    void denser();
    void lessDense();

    void clearField();

private:
    HeightMap *m_heightmap;

    vector<GrassPatch*> m_patches;
    Vector3 m_eye;
};

#endif // GRASSFIELD_H
