#include "color.h"
#include <time.h>
#include <stdlib.h>

/* Create a color */
Color color_create(char r, char g, char b) {
  Color c;
	c.r = r;
	c.g = g;
  c.b = b;
	return c;
}

/* Create a random color */
Color color_create_random() {
  Color c;
  srand(time(NULL));
  c.r = rand() % 256;
  c.g = rand() % 256;
  c.b = rand() % 256;

  return c;
}
