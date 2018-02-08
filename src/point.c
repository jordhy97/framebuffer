#include "point.h"
#include <math.h>

#define PI 3.14159265

/* Create a point */
Point point_create(int x, int y) {
  Point p;
	p.x = x;
	p.y = y;
	return p;
}

/* Scale a point */
Point point_scale(Point p, Point center, float scale_factor) {
  Point result;
  result.x = center.x + (p.x - center.x) * scale_factor;
  result.y = center.y + (p.y - center.y) * scale_factor;
  return result;
}

/* Translate a point */
Point point_translate(Point p, int xoffset, int yoffset) {
  Point result;
  result.x = p.x + xoffset;
  result.y = p.y + yoffset;
  return result;
}

/* Rotate a point (theta in degree) */
Point point_rotate(Point p, Point center, float theta) {
  Point result;
  double cos_theta, sin_theta;

  cos_theta = cos(theta * PI / 180);
  sin_theta = sin(theta * PI / 180);

  result.x = center.x + (p.x - center.x) * cos_theta - (p.y - center.y) * sin_theta;
  result.y = center.y + (p.y - center.y) * cos_theta + (p.x - center.x) * sin_theta;
  return result;
}
