############################################
# Grass Simulator
#
# Authors:
#  David Storch (dstorch@cs.brown.edu)
#  Snow Li (sl90@cs.brown.edu)
#  Zach Wilson (zwilson@cs.brown.edu)
#
# December 2011
#
# Contents:
#   1. Controls
#   2. Features
#   3. Implementation details
#   4. Building and running
#
############################################

############################################
1. Controls
############################################

Movements:
  Mouse click and drag -- move around
  Mouse wheel -- move forward and backward
  Move around -- WASD keys

Interaction:
  Right mouse button -- burst of wind
  J/K -- stronger/weaker bursts

Scene parameters:
  Up/Down -- more/less hilly
  M/L -- more/less grass
  U/I -- more/less ambient wind 

############################################
2. Features                                 
############################################

1. Ambient wind
  A swaying grass effect is acheived by
  perturbing the top vertices of the grass
  quads in the fragment shader.

2. Bursts of wind
  A separate perturbation function which
  decays over time and with distance from
  the wind origin point is used to create
  a burst of wind effect.

3. Fog
  A fragment shader uses depth information
  to blend the grass with a fog color.

4. Sun-lit grass
  When the line of sight to a patch of grass
  is near parallel to the vector from the
  grass to the sun, the grass color is
  blended with a yellowish color. This
  creates the illusion of grass lit by
  the sun.

5. Bloom lighting
 Bloom lighting can be observed when the
 grass is directly in front of the sun.

############################################
3. Implementation details
############################################

1. Procedural terrain
  We did not use recursive vertex perturbation,
  because this technique produces the effect
  of mountainous terrain. Instead, we allocate
  a fixed-size heightmap. Next, we randomly
  choose locations for hill centers. We produce
  a "Gaussian-shaped" hill by adding a Gaussian
  kernel to the current heightmap.

2. Grass cluster geometry
  Each grass cluster consists of size texture-
  mapped quads. There are three intersecting
  quads in an "asterisk" shape, but we must
  render both sides of the quad so that the
  grass can be viewed from any direction. The
  look of grass is acheived by setting the alpha
  to zero for locations on the texture that do
  not contain grass blades.

3. Placing the grass clusters
  The entire field of grass consists of a
  fixed number of grass "patches". Each
  patch covers a fixed area of of the terrain,
  and contains a fixed number of clusters.
  The positions of the clusters are randomized
  within the patch. By arranging the clusters
  into regular patches, we ensure that the
  whole field is covered; by randomizing within
  a patch, we avoid neat rows of grass.

4. Grass motion
  The motion is implemented using a sinusoidal
  function of time and distance. We also divide
  by distance so that the wave is weaker as it
  moves out from its epicenter. For directed
  wind, we take the dot product of the wind
  direction and the vector from the epicenter
  to the cluster. Multiplying by this dot
  product means that the strongest perturbations
  are those which align directly with the wind
  direction. 

############################################
4. Building and running
############################################

QtCreator should be used for generating the Makefile.

