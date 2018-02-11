#include "plane.h"
#include <stdio.h>
#include <stdlib.h>

/* Initialize the plane */
void plane_init(Plane *plane, char *file_path) {
  FILE *file;
  int i, j;

  file = fopen(file_path, "r");
  if (file) {
    while (!feof(file)) {
      fscanf(file, "%d %d\n", &plane->center.x, &plane->center.y);
      plane->center.x *= INIT_SCALE;
      plane->center.y *= INIT_SCALE;
      fscanf(file, "%d\n", &plane->body.neff);
      for (i = 0; i < plane->body.neff; i++) {
        fscanf(file, "%d %d\n", &plane->body.points[i].x, &plane->body.points[i].y);
        plane->body.points[i].x *= INIT_SCALE;
        plane->body.points[i].y *= INIT_SCALE;
      }
      for (i = 0; i < 2; i++) {
        fscanf(file, "%d\n", &plane->mirrors[i].neff);
        for (j = 0; j < plane->mirrors[i].neff; j++) {
          fscanf(file, "%d %d\n", &plane->mirrors[i].points[j].x, &plane->mirrors[i].points[j].y);
          plane->mirrors[i].points[j].x *= INIT_SCALE;
          plane->mirrors[i].points[j].y *= INIT_SCALE;
        }
      }
      for (i = 0; i < 2; i++) {
        fscanf(file, "%d\n", &plane->wings[i].neff);
        for (j = 0; j < plane->wings[i].neff; j++) {
          fscanf(file, "%d %d\n", &plane->wings[i].points[j].x, &plane->wings[i].points[j].y);
          plane->wings[i].points[j].x *= INIT_SCALE;
          plane->wings[i].points[j].y *= INIT_SCALE;
        }
      }
      for (i = 0; i < 3; i++) {
        fscanf(file, "%d\n", &plane->tails[i].neff);
        for (j = 0; j < plane->tails[i].neff; j++) {
          fscanf(file, "%d %d\n", &plane->tails[i].points[j].x, &plane->tails[i].points[j].y);
          plane->tails[i].points[j].x *= INIT_SCALE;
          plane->tails[i].points[j].y *= INIT_SCALE;
        }
      }
      for (i = 0; i < 8; i++) {
        fscanf(file, "%d\n", &plane->propellers[i].neff);
        for (j = 0; j < plane->propellers[i].neff; j++) {
          fscanf(file, "%d %d\n", &plane->propellers[i].points[j].x, &plane->propellers[i].points[j].y);
          plane->propellers[i].points[j].x *= INIT_SCALE;
          plane->propellers[i].points[j].y *= INIT_SCALE;
        }
      }
      for (i = 0; i < 3; i++) {
        fscanf(file, "%d\n", &plane->wheel_connectors[i].neff);
        for (j = 0; j < plane->wheel_connectors[i].neff; j++) {
          fscanf(file, "%d %d\n", &plane->wheel_connectors[i].points[j].x, &plane->wheel_connectors[i].points[j].y);
          plane->wheel_connectors[i].points[j].x *= INIT_SCALE;
          plane->wheel_connectors[i].points[j].y *= INIT_SCALE;
        }
      }
      for (i = 0; i < 3; i++) {
        fscanf(file, "%d\n", &plane->wheels[i].neff);
        for (j = 0; j < plane->wheels[i].neff; j++) {
          fscanf(file, "%d %d\n", &plane->wheels[i].points[j].x, &plane->wheels[i].points[j].y);
          plane->wheels[i].points[j].x *= INIT_SCALE;
          plane->wheels[i].points[j].y *= INIT_SCALE;
        }
        plane->wheel_fall_speed[i] = INIT_FALL_SPEED;
        plane->wheel_fall_limit[i] = INIT_FALL_LIMIT;
      }
    }
    fclose(file);
    plane->hit_counter = 0;
  } else {
    perror("Error: failed to load plane");
    exit(6);
  }
}


/* Set plane new center */
void plane_set_center(Plane *plane, Point new_center) {
  int i, j, xoffset, yoffset;

  xoffset = new_center.x - plane->center.x;
  yoffset = new_center.y - plane->center.y;
  plane->center = new_center;
  for (i = 0; i < plane->body.neff; i++) {
    plane->body.points[i] = point_translate(plane->body.points[i], xoffset, yoffset);
  }
  for (i = 0; i < 2; i++) {
    for (j = 0; j < plane->mirrors[i].neff; j++) {
        plane->mirrors[i].points[j] = point_translate(plane->mirrors[i].points[j], xoffset, yoffset);
    }
  }
  for (i = 0; i < 2; i++) {
    for (j = 0; j < plane->wings[i].neff; j++) {
      plane->wings[i].points[j] = point_translate(plane->wings[i].points[j], xoffset, yoffset);
    }
  }
  for (i = 0; i < 3; i++) {
    for (j = 0; j < plane->tails[i].neff; j++) {
      plane->tails[i].points[j] = point_translate(plane->tails[i].points[j], xoffset, yoffset);
    }
  }
  for (i = 0; i < 8; i++) {
    for (j = 0; j < plane->propellers[i].neff; j++) {
      plane->propellers[i].points[j] = point_translate(plane->propellers[i].points[j], xoffset, yoffset);
    }
  }
  for (i = 0; i < 3; i++) {
    for (j = 0; j < plane->wheel_connectors[i].neff; j++) {
      plane->wheel_connectors[i].points[j] = point_translate(plane->wheel_connectors[i].points[j], xoffset, yoffset);
    }
  }
  for (i = 0; i < 3; i++) {
    for (j = 0; j < plane->wheels[i].neff; j++) {
      plane->wheels[i].points[j] = point_translate(plane->wheels[i].points[j], xoffset, yoffset);
    }
  }
}

/* Scale plane */
void plane_scale(Plane *plane, double scale) {
  int i, j;

  for (i = 0; i < plane->body.neff; i++) {
    plane->body.points[i] = point_scale(plane->body.points[i], plane->center, scale);
  }
  for (i = 0; i < 2; i++) {
    for (j = 0; j < plane->mirrors[i].neff; j++) {
        plane->mirrors[i].points[j] = point_scale(plane->mirrors[i].points[j], plane->center, scale);
    }
  }
  for (i = 0; i < 2; i++) {
    for (j = 0; j < plane->wings[i].neff; j++) {
      plane->wings[i].points[j] = point_scale(plane->wings[i].points[j], plane->center, scale);
    }
  }
  for (i = 0; i < 3; i++) {
    for (j = 0; j < plane->tails[i].neff; j++) {
      plane->tails[i].points[j] = point_scale(plane->tails[i].points[j], plane->center, scale);
    }
  }
  for (i = 0; i < 8; i++) {
    for (j = 0; j < plane->propellers[i].neff; j++) {
      plane->propellers[i].points[j] = point_scale(plane->propellers[i].points[j], plane->center, scale);
    }
  }
  for (i = 0; i < 3; i++) {
    for (j = 0; j < plane->wheel_connectors[i].neff; j++) {
      plane->wheel_connectors[i].points[j] = point_scale(plane->wheel_connectors[i].points[j], plane->center, scale);
    }
  }
  for (i = plane->hit_counter; i < 3; i++) {
    for (j = 0; j < plane->wheels[i].neff; j++) {
      plane->wheels[i].points[j] = point_scale(plane->wheels[i].points[j], plane->center, scale);
    }
  }
}

/* Rotate plane propellers*/
void plane_rotate_propellers(Plane *plane) {
  int i , j;
  for (i = 0; i < 8; i++) {
    for (j = 0; j < plane->propellers[i].neff; j++) {
      plane->propellers[i].points[j] = point_rotate(plane->propellers[i].points[j], plane->propellers[i].points[0], 45);
    }
  }
}

/* Move plane wheel (shot) */
void plane_move_wheel(Plane *plane) {
  int i, j;
  for (i = 0; i < plane->hit_counter; i++) {
    if (plane->wheel_fall_limit[i] > 0) {
      if (plane->wheel_fall_speed[i] == plane->wheel_fall_limit[i] * INIT_FALL_SPEED) {
        plane->wheel_fall_limit[i]--;
        plane->wheel_fall_speed[i] *= -1;
        plane->wheel_fall_speed[i] += (2 * INIT_FALL_SPEED);
      }
      for (j = 0; j < plane->wheels[i].neff; j++) {
        plane->wheels[i].points[j].y += plane->wheel_fall_speed[i];
        if (i == 2) {
        plane->wheels[i].points[j].x -= 5;
        } else {
          plane->wheels[i].points[j].x += 5;
        }
      }
      plane->wheel_fall_speed[i] += INIT_FALL_SPEED;
    }
  }
}
