#ifndef POINT_H
#define POINT_H

typedef struct {
	int x; /* absis   */
	int y; /* ordinat */
} Point;

/* Create a point */
Point point_create(int x, int y);

/* Scale up a point */
Point point_scale_up(Point p, int scale);

/* Translate a point */
Point point_translate(Point p, int xoffset, int yoffset);

#endif
