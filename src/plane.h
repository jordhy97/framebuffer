#ifndef PLANE_H
#define PLANE_H

#define INIT_SCALE 3
#define INIT_FALL_SPEED 5
#define INIT_FALL_LIMIT 10

#include "polygon.h"
#include "point.h"

typedef struct {
  Point center;
	Polygon body;
  Polygon mirrors[2];
  Polygon wings[2];
  Polygon tails[3];
  Polygon propellers[8];
  Polygon wheel_connectors[3];
  Polygon wheels[3];
  int hit_counter;
  int wheel_fall_speed[3];
  int wheel_fall_limit[3];
} Plane;

/* Initialize the plane */
void plane_init(Plane *plane, char *file_path);

/* Set plane new center */
void plane_set_center(Plane *plane, Point new_center);

/* Scale plane */
void plane_scale(Plane *plane, double scale);

/* Rotate plane propellers*/
void plane_rotate_propellers(Plane *plane);

/* Move plane wheel (shot) */
void plane_move_wheel(Plane *plane);
#endif
