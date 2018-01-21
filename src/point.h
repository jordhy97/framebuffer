#ifndef POINT_H
#define POINT_H

typedef struct {
	int x; /* absis   */
	int y; /* ordinat */
} Point;

/* Create a point */
Point point_create(int x, int y);

#endif
