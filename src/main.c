#include "framebuffer.h"
#include "color.h"
#include "point.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>

#define FONT_HEIGHT 60
#define FONT_WIDTH 40
#define VERTICAL_SPACE 15
#define HORIZONTAL_SPACE 7
#define SCALE 10
#define ROCKET_SCALE 25
#define MAX_GUNFIRES 1000

typedef struct {
  int pair;
  Point points[15][2];
} Alphabet;

typedef struct {
  Point start;
  Point end;
  int x_speed;
  int y_speed;
} Gunfire;

Gunfire gunfire_types[3];

void init_gunfires(int xres, int yres) {
  gunfire_types[0].start = point_create(xres / 2 - 1, yres - 1);
  gunfire_types[1].start = point_create(xres / 2 - 1, yres - 1);
  gunfire_types[2].start = point_create(xres / 2 - 1, yres - 1);

  gunfire_types[0].x_speed = 25;
  gunfire_types[1].x_speed = 0;
  gunfire_types[2].x_speed = -25;

  gunfire_types[0].y_speed = -30;
  gunfire_types[1].y_speed = -30;
  gunfire_types[2].y_speed = -30;

  gunfire_types[0].end = point_create(xres / 2 + 14, yres - 21);
  gunfire_types[1].end = point_create(xres / 2 - 1, yres - 21);
  gunfire_types[2].end = point_create(xres / 2 - 16, yres - 21);


}

int main()
{
  Framebuffer fb;

  FILE *font;
  Alphabet alphabets[26];
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

  int counter = -1;
  int line = 0;
  int start, i, j, xoffset, yoffset, len, x1, y1, x2, y2;
  char letter;
  struct termios old_tio, new_tio;

  char c='D';

  Gunfire gunfires[MAX_GUNFIRES];
  int gunfires_neff = 0;
  int head = 0;
  int tail = -1;
  Alphabet rocket;
  int rocket_offset;

  fb_init(&fb, "/dev/fb0");
  init_gunfires(fb.vinfo.xres, fb.vinfo.yres);
  rocket_offset = fb.vinfo.xres;

  // Load font
  font = fopen("../data/font2.txt", "r");
  if (font) {
    while (!feof(font)) {
      fscanf(font, "\n%c ", &letter);
      fscanf(font, "%d", &alphabets[letter - 'A'].pair);
      for (i = 0; i < alphabets[letter - 'A'].pair; i++) {
        fscanf(font, "%d %d %d %d", &x1, &y1, &x2, &y2);
        alphabets[letter - 'A'].points[i][0] = point_create(x1, y1);
        alphabets[letter - 'A'].points[i][1] = point_create(x2, y2);
      }
    }
    fclose(font);
  }

  // Load rocket
  font = fopen("../data/rocket.txt", "r");
  if (font) {
    while (!feof(font)) {
      fscanf(font, "%d\n", &rocket.pair);
      for (i = 0; i < rocket.pair; i++) {
        fscanf(font, "%d %d %d %d\n", &x1, &y1, &x2, &y2);
        rocket.points[i][0] = point_create(x1, y1);
        rocket.points[i][1] = point_create(x2, y2);
      }
    }
    fclose(font);
  }

  start = fb.vinfo.yres;

  /* get the terminal settings for stdin */
  tcgetattr(STDIN_FILENO,&old_tio);

  /* we want to keep the old setting to restore them a the end */
  new_tio = old_tio;

  /* disable canonical mode (buffered i/o) and local echo */
  new_tio.c_lflag &=(~ICANON & ~ECHO);

  /* set the new settings immediately */
  tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);

  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking

  while (start >= 0) {
    fb_clear(&fb);
    for (line = 0; line < 7; line++) {
      len = strlen(names[line]);
      for (i = 0; i < len; i++) {
        yoffset = start + line * (FONT_HEIGHT + VERTICAL_SPACE);
        xoffset = i * (FONT_WIDTH + HORIZONTAL_SPACE) + ((fb.vinfo.xres - (len * (FONT_WIDTH + HORIZONTAL_SPACE) - HORIZONTAL_SPACE)) / 2);
        if (names[line][i] != ' ') {
          for (j = 0; j < alphabets[names[line][i] - 'A'].pair; j++) {
            fb_draw_line(&fb, point_translate(point_scale_up(alphabets[names[line][i] - 'A'].points[j][0], SCALE), xoffset, yoffset),
            point_translate(point_scale_up(alphabets[names[line][i] - 'A'].points[j][1], SCALE), xoffset, yoffset), names_color[line]);
          }
        }
      }
    }
    start -= 5;
    read(STDIN_FILENO,&c,1);
     if (c == ' ') {
       tail++;
       counter = (counter + 1) % 3;
       gunfires[tail] = gunfire_types[counter];
       c = 'b';
       gunfires_neff++;
     }
    tcflush(0, TCIFLUSH);

    /* Draw rocket */
    for (i = 0; i < rocket.pair; i++) {
      fb_draw_line(&fb, point_translate(point_scale_up(rocket.points[i][0], ROCKET_SCALE), rocket_offset, 0),
      point_translate(point_scale_up(rocket.points[i][1], ROCKET_SCALE), rocket_offset, 0), color_create(0, 255, 255));
    }
    rocket_offset -= 9;

    /* Draw gunfires */
    for (i = head; i < head + gunfires_neff; i++) {
      fb_draw_line(&fb, gunfires[i].start, gunfires[i].end, color_create(192, 192, 192));
      gunfires[i].start = point_translate(gunfires[i].start, gunfires[i].x_speed, gunfires[i].y_speed);
      gunfires[i].end = point_translate(gunfires[i].end, gunfires[i].x_speed, gunfires[i].y_speed);
      if (gunfires[i].start.y < 0) {
        head = (head + 1) % MAX_GUNFIRES;
        gunfires_neff--;
      }
    }

    fb_display(&fb);
  }

  tcflush(0, TCIFLUSH);

  /* restore the former settings */
  tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);

  /* Close the framebuffer */
  fb_close(&fb);

  return 0;
}
