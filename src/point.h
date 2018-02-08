#ifndef POINT_H
#define POINT_H

typedef struct {
	int x; /* absis   */
	int y; /* ordinat */
} Point;

/* Create a point */
Point point_create(int x, int y);

/* Scale a point */
Point point_scale(Point p, Point center, float scale_factor);

/* Translate a point */
Point point_translate(Point p, int xoffset, int yoffset);

/* Rotate a point (theta in degree) */
Point point_rotate(Point p, Point center, float theta);

#endif
