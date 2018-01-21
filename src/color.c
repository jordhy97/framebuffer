#include "color.h"

/* Create a color */
Color color_create(char r, char g, char b) {
  Color c;
	c.r = r;
	c.g = g;
  c.b = b;
	return c;
}
