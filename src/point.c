#include "point.h"

/* Create a point */
Point point_create(int x, int y) {
  Point p;
	p.x = x;
	p.y = y;
	return p;
}
