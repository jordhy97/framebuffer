#include "cannonball.h"
#include <stdio.h>
#include <stdlib.h>

/* Initialize the cannon ball */
void cannon_ball_init(CannonBall *cannon_ball, char *file_path, int x_speed, int y_speed) {
  FILE *file;
  int i;

  cannon_ball->x_min = 10000;
  cannon_ball->y_min = 10000;
  cannon_ball->x_max = 0;
  cannon_ball->y_max = 0;
  file = fopen(file_path, "r");
  if (file) {
    while (!feof(file)) {
      fscanf(file, "%d %d\n", &cannon_ball->center.x, &cannon_ball->center.y);
      cannon_ball->center.x *= CANNON_BALL_INIT_SCALE;
      cannon_ball->center.y *= CANNON_BALL_INIT_SCALE;
      fscanf(file, "%d\n", &cannon_ball->body.neff);
      for (i = 0; i < cannon_ball->body.neff; i++) {
        fscanf(file, "%d %d\n", &cannon_ball->body.points[i].x, &cannon_ball->body.points[i].y);
        cannon_ball->body.points[i].x *= CANNON_BALL_INIT_SCALE;
        cannon_ball->body.points[i].y *= CANNON_BALL_INIT_SCALE;
        if (cannon_ball->body.points[i].x < cannon_ball->x_min) {
          cannon_ball->x_min = cannon_ball->body.points[i].x;
        }
        if (cannon_ball->body.points[i].x > cannon_ball->x_max) {
          cannon_ball->x_max = cannon_ball->body.points[i].x;
        }
        if (cannon_ball->body.points[i].y < cannon_ball->y_min) {
          cannon_ball->y_min = cannon_ball->body.points[i].y;
        }
        if (cannon_ball->body.points[i].x > cannon_ball->y_max) {
          cannon_ball->y_max = cannon_ball->body.points[i].y;
        }
      }
    }
    fclose(file);
    cannon_ball->x_speed = x_speed;
    cannon_ball->y_speed = y_speed;
    cannon_ball->counter = 0;
    cannon_ball->destroyed = 0;
  } else {
    perror("Error: failed to load plane");
    exit(6);
  }
}

/* Set cannon ball new center */
void cannon_ball_set_center(CannonBall *cannon_ball, Point new_center) {
  int i, xoffset, yoffset;

  xoffset = new_center.x - cannon_ball->center.x;
  yoffset = new_center.y - cannon_ball->center.y;
  cannon_ball->center = new_center;
  for (i = 0; i < cannon_ball->body.neff; i++) {
    cannon_ball->body.points[i] = point_translate(cannon_ball->body.points[i], xoffset, yoffset);
  }
  cannon_ball->x_min += xoffset;
  cannon_ball->x_max += xoffset;
  cannon_ball->y_min += yoffset;
  cannon_ball->y_max += yoffset;
}

/* Scale cannon ball */
void cannon_ball_scale(CannonBall *cannon_ball, double scale) {
  int i;

  for (i = 0; i < cannon_ball->body.neff; i++) {
    cannon_ball->body.points[i] = point_scale(cannon_ball->body.points[i], cannon_ball->center, scale);
  }
  cannon_ball->x_min = cannon_ball->center.x + (cannon_ball->x_min - cannon_ball->center.x) * scale;
  cannon_ball->x_max = cannon_ball->center.x + (cannon_ball->x_max - cannon_ball->center.x) * scale;
  cannon_ball->y_min = cannon_ball->center.y + (cannon_ball->y_min - cannon_ball->center.y) * scale;
  cannon_ball->y_max = cannon_ball->center.y + (cannon_ball->y_max - cannon_ball->center.y) * scale;
}


/* Move cannon ball */
void cannon_ball_move(CannonBall *cannon_ball) {
  int i;

  cannon_ball->counter++;
  if (cannon_ball->counter == 35) {
    cannon_ball->destroyed = 1;
  } else {
    cannon_ball->center = point_translate(cannon_ball->center, cannon_ball->x_speed, cannon_ball->y_speed);
    for (i = 0; i < cannon_ball->body.neff; i++) {
        cannon_ball->body.points[i] = point_translate(cannon_ball->body.points[i], cannon_ball->x_speed, cannon_ball->y_speed);
    }
    if (cannon_ball->counter % 10 == 0) {
      cannon_ball_scale(cannon_ball, 0.5);
    }
    cannon_ball->y_speed += SPEED_RATE;
    cannon_ball->x_min += cannon_ball->x_speed;
    cannon_ball->x_max += cannon_ball->x_speed;
    cannon_ball->y_min += cannon_ball->y_speed;
    cannon_ball->y_max += cannon_ball->y_speed;
  }
}
