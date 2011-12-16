/*!
   @file   Constants.cpp
   @author dstorch@cs.brown.edu
   @author sl90@cs.brown.edu
   @author zwilson@cs.brown.edu
   @date   December 2011
*/

#include "Constants.h"

Constants constants;

void Constants::initializeConstants()
{
    clustersPerPatch = CLUSTERS_PER_PATCH;

    hills = HILLS;
    sigma2x = SIGMA2X;
    sigma2y = SIGMA2Y;
    a = A;

    perturbationWaveAmplitude = 5.0;
    ambientWaveAmplitude = .4;
}
