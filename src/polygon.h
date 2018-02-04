#ifndef POLYGON_H
#define POLYGON_H

#define MAX_HEIGHT 70
#define MAX_WIDTH 50
#define MAX_NUM_OF_X 30
#define MAX_NUM_OF_Y 100

#include "point.h"
#include "color.h"

typedef struct {
  Point points[30];
  int neff;
  int height;
  int width;
  Color canvas[MAX_HEIGHT][MAX_WIDTH];
} Polygon;

typedef struct {
  int intersections[MAX_NUM_OF_Y][MAX_NUM_OF_X];
  int neff[MAX_NUM_OF_Y];
} Intersections;

/* Initialize a polygon */
void polygon_init(Polygon *polygon, Color border_color, Color fill_color);

/* Draw polygon line */
void polygon_draw_line(Polygon *polygon, Point start, Point end, Intersections *intersections, Color color);

/* Draw polygon line */
void polygon_draw_line_low(Polygon *polygon, Point start, Point end, Intersections *intersections, Color color);

/* Draw polygon line */
void polygon_draw_line_high(Polygon *polygon, Point start, Point end, Intersections *intersections, Color color);

#endif
