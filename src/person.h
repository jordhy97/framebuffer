#ifndef PERSON_H
#define PERSON_H

#define PERSON_X_SPEED -15

#include "polygon.h"
#include "point.h"

typedef struct {
  Point center;
  Polygon person;
  Polygon parachute;
  Polygon parachute_strings[4];
  int y_speed;
} Person;

/* Initialize the person */
void person_init(Person *person, char *file_path, int y_speed);

/* Set person new center */
void person_set_center(Person *person, Point new_center);

/* Scale person */
void person_scale(Person *person, double scale);

/* Move person */
void person_move(Person *person);

#endif
