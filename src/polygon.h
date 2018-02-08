#ifndef POLYGON_H
#define POLYGON_H

#define MAX_NUM_OF_X 30
#define MAX_NUM_OF_Y 1000

#include "point.h"
#include "color.h"

typedef struct {
  Point points[30];
  int neff;
} Polygon;

typedef struct {
  int intersections[MAX_NUM_OF_Y][MAX_NUM_OF_X];
  int neff[MAX_NUM_OF_Y];
} Intersections;

#endif
