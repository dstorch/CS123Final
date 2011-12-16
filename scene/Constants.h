/*!
   @file   Constants.h
   @author dstorch@cs.brown.edu
   @author sl90@cs.brown.edu
   @author zwilson@cs.brown.edu
   @date   December 2011
*/

#ifndef CONSTANTS_H
#define CONSTANTS_H

// HeightMap Gaussian Distr. Constants
#define HILLS 50.0
#define RADII 1000.0
#define SIGMA2X 50.0
#define SIGMA2Y 50.0
#define A 4.0

// glwidgetConstants
#define TERRAIN_HEIGHT 150
#define TERRAIN_WIDTH 150

// camera motion constants
#define CAM_TRANSLATE_SPEED 1.5
#define CAM_WHEEL_SENSITIVITY 80.0

// swaying grass animation parameters
#define SWAY_SPEED 0.001
#define TIME_ATT 0.025
#define INITIAL_TIME 100.53096148 // 32 * pi

#define CLUSTERS_PER_PATCH 4
#define PATCH_SIZE 2

/**

 @struct Constants

 @brief Holds parameters
 that might change during
 execution.

**/
struct Constants
{
    void initializeConstants();

    int hills;
    float sigma2x;
    float sigma2y;
    float a;

    float clustersPerPatch;

    float perturbationWaveAmplitude;
    float ambientWaveAmplitude;
};

extern Constants constants;


#endif // CONSTANTS_H
