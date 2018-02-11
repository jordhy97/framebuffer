#ifndef CANNON_BALL_H
#define CANNON_BALL_H

#define CANNON_BALL_INIT_SCALE 4
#define SPEED_RATE 3

#include "polygon.h"
#include "point.h"

typedef struct {
  Point center;
	Polygon body;
  int y_speed;
  int x_speed;
  int counter;
  int x_min;
  int x_max;
  int y_min;
  int y_max;
  int destroyed;
} CannonBall;

/* Initialize the cannon ball */
void cannon_ball_init(CannonBall *cannon_ball, char *file_path, int x_speed, int y_speed);

/* Set cannon ball new center */
void cannon_ball_set_center(CannonBall *cannon_ball, Point new_center);

/* Scale cannon ball */
void cannon_ball_scale(CannonBall *cannon_ball, double scale);

/* Move cannon ball */
void cannon_ball_move(CannonBall *cannon_ball);

#endif
