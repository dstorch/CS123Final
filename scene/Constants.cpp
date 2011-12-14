
#include "Constants.h"

Constants constants;

void Constants::initializeConstants()
{
    clustersPerPatch = CLUSTERS_PER_PATCH;

    hills = HILLS;
    sigma2x = SIGMA2X;
    sigma2y = SIGMA2Y;
    a = A;
}
