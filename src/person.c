#include "person.h"
#include <stdio.h>
#include <stdlib.h>

/* Initialize the person */
void person_init(Person *person, char *file_path, int y_speed) {
  FILE *file;
  int i, j;

  file = fopen(file_path, "r");
  if (file) {
    while (!feof(file)) {
      fscanf(file, "%d %d\n", &person->center.x, &person->center.y);
      fscanf(file, "%d\n", &person->person.neff);
      for (i = 0; i < person->person.neff; i++) {
        fscanf(file, "%d %d\n", &person->person.points[i].x, &person->person.points[i].y);
      }
      fscanf(file, "%d\n", &person->parachute.neff);
      for (i = 0; i < person->parachute.neff; i++) {
        fscanf(file, "%d %d\n", &person->parachute.points[i].x, &person->parachute.points[i].y);
      }
      for (i = 0; i < 4; i++) {
        fscanf(file, "%d\n", &person->parachute_strings[i].neff);
        for (j = 0; j < person->parachute_strings[i].neff; j++) {
          fscanf(file, "%d %d\n", &person->parachute_strings[i].points[j].x, &person->parachute_strings[i].points[j].y);
        }
      }
    }
    fclose(file);
    person->y_speed = y_speed;
  } else {
    perror("Error: failed to load plane");
    exit(6);
  }
}

/* Set person new center */
void person_set_center(Person *person, Point new_center) {
  int i, j, xoffset, yoffset;

  xoffset = new_center.x - person->center.x;
  yoffset = new_center.y - person->center.y;
  person->center = new_center;
  for (i = 0; i < person->person.neff; i++) {
    person->person.points[i] = point_translate(person->person.points[i], xoffset, yoffset);
  }
  for (i = 0; i < person->parachute.neff; i++) {
    person->parachute.points[i] = point_translate(person->parachute.points[i], xoffset, yoffset);
  }
  for (i = 0; i < 4; i++) {
    for (j = 0; j < person->parachute_strings[i].neff; j++) {
        person->parachute_strings[i].points[j] = point_translate(person->parachute_strings[i].points[j], xoffset, yoffset);
    }
  }
}

/* Scale person */
void person_scale(Person *person, double scale) {
  int i, j;

  for (i = 0; i < person->person.neff; i++) {
    person->person.points[i] = point_scale(person->person.points[i], person->center, scale);
  }
  for (i = 0; i < person->parachute.neff; i++) {
    person->parachute.points[i] = point_scale(person->parachute.points[i], person->center, scale);
  }
  for (i = 0; i < 4; i++) {
    for (j = 0; j < person->parachute_strings[i].neff; j++) {
        person->parachute_strings[i].points[j] = point_scale(person->parachute_strings[i].points[j], person->center, scale);
    }
  }

}

/* Move person */
void person_move(Person *person) {
  int i, j, x_offset;

  if (person->y_speed > 0) {
    person->y_speed = 2;
    x_offset = rand() % 11 - 5;
  } else {
    x_offset = PERSON_X_SPEED;
  }
  for (i = 0; i < person->person.neff; i++) {
    person->person.points[i] = point_translate(person->person.points[i], x_offset, person->y_speed);
  }
  for (i = 0; i < person->parachute.neff; i++) {
    person->parachute.points[i] = point_translate(person->parachute.points[i], x_offset, person->y_speed);
  }
  for (i = 0; i < 4; i++) {
    for (j = 0; j < person->parachute_strings[i].neff; j++) {
      person->parachute_strings[i].points[j] = point_translate(person->parachute_strings[i].points[j], x_offset, person->y_speed);
    }
  }
  person->y_speed += 5;
}
