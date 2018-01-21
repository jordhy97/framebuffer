#include "framebuffer.h"
#include "color.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define FONT_HEIGHT 56
#define FONT_WIDTH 40
#define VERTICAL_SPACE 15
#define HORIZONTAL_SPACE 7

int main()
{
  Framebuffer fb;

  FILE *font;
  char alphabets[26][FONT_HEIGHT][FONT_WIDTH];
  char names[7][50] = {
    "ADITYA PRATAMA",
    "JORDHY FERNANDO",
    "MUHAMMAD FARHAN KEMAL",
    "SYLVIA JULIANA",
    "TASYA",
    "TURFA AULIARACHMAN",
    "VIGOR AKBAR"
  };
  Color names_color[7] = {
    COLOR_RED,
    COLOR_ORANGE,
    COLOR_YELLOW,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_INDIGO,
    COLOR_VIOLET
  };

  int counter = 0;
  int line = 0;
  int start, i, x, y, xoffset, yoffset, len;

  fb_init(&fb, "/dev/fb0");

  // Load font
  font = fopen("../data/font.txt", "r");
  if (font) {
    while (fscanf(font, "%s", alphabets[counter][line]) != EOF) {
      line++;
      if (line == FONT_HEIGHT) {
        counter++;
        line = 0;
      }
    }
    fclose(font);
  }

  start = fb.vinfo.yres;

  while (start >= 0) {
    fb_clear(&fb);
    for (line = 0; line < 7; line++) {
      len = strlen(names[line]);
      for (i = 0; i < len; i++) {
        yoffset = line * (FONT_HEIGHT + VERTICAL_SPACE);
        for (y = start + yoffset; y < start + yoffset + FONT_HEIGHT; y++) {
          xoffset = i * (FONT_WIDTH + HORIZONTAL_SPACE) + ((fb.vinfo.xres - (len * (FONT_WIDTH + HORIZONTAL_SPACE) - HORIZONTAL_SPACE)) / 2);
          for (x = xoffset; x < xoffset + FONT_WIDTH; x++) {
            if (names[line][i] != ' ' && alphabets[names[line][i] - 'A'][y - (start + yoffset)][x - xoffset] == '1') {
              fb_draw_pixel(&fb, point_create(x, y), names_color[line]);
            }
          }
        }
      }
    }
    start -= 5;
    fb_display(&fb);
  }
  fb_close(&fb);
  return 0;
}
