#include "point.h"

/* Create a point */
Point point_create(int x, int y) {
  Point p;
	p.x = x;
	p.y = y;
	return p;
}


/* Scale up a point */
Point point_scale_up(Point p, int scale) {
  Point result;
  result.x = p.x * scale;
  result.y = p.y * scale;
  return result;
}

/* Translate a point */
Point point_translate(Point p, int xoffset, int yoffset) {
  Point result;
  result.x = p.x + xoffset;
  result.y = p.y + yoffset;
  return result;
}
