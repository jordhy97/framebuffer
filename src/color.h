#ifndef COLOR_H
#define COLOR_H

#define COLOR_BLACK ((Color) { 0, 0, 0 })
#define COLOR_RED ((Color) {255, 0, 0})
#define COLOR_ORANGE ((Color) {255, 127, 0})
#define COLOR_YELLOW ((Color) {255, 255, 0})
#define COLOR_GREEN ((Color) {0, 255, 0})
#define COLOR_BLUE ((Color) {0, 0, 255})
#define COLOR_INDIGO ((Color) {75, 0, 130})
#define COLOR_VIOLET ((Color) {148, 0, 211})
#define COLOR_WHITE ((Color) {255, 255, 255})

typedef struct {
  char r;
  char g;
  char b;
} Color;

/* Create a color */
Color color_create(char r, char g, char b);

#endif
