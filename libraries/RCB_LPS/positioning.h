#ifndef ROBOT_CONTROL_BOARD_LPS_POSITIONING_H
#define ROBOT_CONTROL_BOARD_LPS_POSITIONING_H

#include <Smoothed.h>
#include "config.h"
#include "containers.h"

boolean getLpsXyz()
{
  // https://en.wikipedia.org/wiki/True-range_multilateration#Three_Cartesian_dimensions,_three_measured_slant_ranges

  // Distances from this tag to anchors 1, 2, 3
  float r1 = distToAnchorSmoothed1.get(); // distancesToAnchors[0];
  float r2 = distToAnchorSmoothed2.get(); // distancesToAnchors[1];
  float r3 = distToAnchorSmoothed3.get(); // distancesToAnchors[2];

  lpsX = ((r1 * r1) - (r2 * r2) + (D12 * D12)) / (2.0 * D12);
  lpsY = ((r1 * r1) - (r3 * r3) + (A3X * A3X) + (A3Y * A3Y) - (2.0 * A3X * lpsX)) / (2.0 * A3Y);
  float z2 = r1 * r1 - lpsX * lpsX - lpsY * lpsY;
  lpsZ = z2 >= 0 ? -sqrt(z2) : 0;
  // lpsZ = sqrt(r1 * r1 - lpsX * lpsX - lpsY * lpsY);

  return (r1 < maxDistance[0] && r1 > minDistance[0]) && (r2 < maxDistance[1] && r2 > minDistance[1]) && (r3 < maxDistance[2] && r3 > minDistance[2]);
}

#endif