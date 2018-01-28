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

typedef struct {
  int pair;
  Point points[15][2];
  int x_speed;
  int y_speed;
  int x_borders[2];
  int y_borders[2];
  int destroyed;
} Rocket;

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

  FILE *file;
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
  Rocket rocket;

  fb_init(&fb, "/dev/fb0");
  init_gunfires(fb.vinfo.xres, fb.vinfo.yres);

  // Load font
  file = fopen("../data/font2.txt", "r");
  if (file) {
    while (!feof(file)) {
      fscanf(file, "\n%c ", &letter);
      fscanf(file, "%d", &alphabets[letter - 'A'].pair);
      for (i = 0; i < alphabets[letter - 'A'].pair; i++) {
        fscanf(file, "%d %d %d %d", &x1, &y1, &x2, &y2);
        alphabets[letter - 'A'].points[i][0] = point_create(x1, y1);
        alphabets[letter - 'A'].points[i][1] = point_create(x2, y2);
      }
    }
    fclose(file);
  }

  // Load and init rocket
  file = fopen("../data/rocket.txt", "r");
  if (file) {
    while (!feof(file)) {
      fscanf(file, "%d\n", &rocket.pair);
      for (i = 0; i < rocket.pair; i++) {
        fscanf(file, "%d %d %d %d\n", &x1, &y1, &x2, &y2);
        rocket.points[i][0] = point_translate(point_scale_up(point_create(x1, y1), ROCKET_SCALE), fb.vinfo.xres, 0);
        rocket.points[i][1] = point_translate(point_scale_up(point_create(x2, y2), ROCKET_SCALE), fb.vinfo.xres, 0);
      }
      fscanf(file, "%d %d\n", &rocket.x_borders[0], &rocket.x_borders[1]);
      fscanf(file, "%d %d\n", &rocket.y_borders[0], &rocket.y_borders[1]);
      rocket.x_borders[0] = rocket.x_borders[0] * ROCKET_SCALE + fb.vinfo.xres;
      rocket.x_borders[1] = rocket.x_borders[1] * ROCKET_SCALE + fb.vinfo.xres;
      rocket.y_borders[0] = rocket.y_borders[0] * ROCKET_SCALE;
      rocket.y_borders[1] = rocket.y_borders[1] * ROCKET_SCALE;
      rocket.x_speed = -9;
      rocket.y_speed = 0;
      rocket.destroyed = -1;
    }
    fclose(file);
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
            fb_draw__dotted_line(&fb, point_translate(point_scale_up(alphabets[names[line][i] - 'A'].points[j][0], SCALE), xoffset, yoffset),
            point_translate(point_scale_up(alphabets[names[line][i] - 'A'].points[j][1], SCALE), xoffset, yoffset), 3, names_color[line]);
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

    /* Check collision */
    i = head;
    while (rocket.destroyed == -1 && i < head + 3 && i < head + gunfires_neff) {
      if ((rocket.x_borders[0] <= gunfires[i].start.x && gunfires[i].start.x <= rocket.x_borders[1] &&
          rocket.y_borders[0] <= gunfires[i].start.y && gunfires[i].start.y <= rocket.y_borders[1]) ||
          (rocket.x_borders[0] <= gunfires[i].end.x && gunfires[i].end.x <= rocket.x_borders[1] &&
          rocket.y_borders[0] <= gunfires[i].end.y && gunfires[i].end.y <= rocket.y_borders[1])) {
            rocket.destroyed = 1;
            gunfires[i].start = point_translate(gunfires[i].start, gunfires[i].x_speed, -1000);
            gunfires[i].end = point_translate(gunfires[i].end, gunfires[i].x_speed, -1000);
          }
      i++;
    }

    /* Draw rocket */
    if (rocket.destroyed == -1) {
      for (i = 0; i < rocket.pair; i++) {
        fb_draw_line(&fb, rocket.points[i][0], rocket.points[i][1], COLOR_CYAN);
        rocket.points[i][0] = point_translate(rocket.points[i][0], rocket.x_speed, 0);
        rocket.points[i][1] = point_translate(rocket.points[i][1], rocket.x_speed, 0);
      }
      rocket.x_borders[0] += rocket.x_speed;
      rocket.x_borders[1] += rocket.x_speed;
    }

    /* Draw gunfires */
    for (i = head; i < head + gunfires_neff; i++) {
      fb_draw_line(&fb, gunfires[i].start, gunfires[i].end, COLOR_SILVER);
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
  // for (i = head; i < head + gunfires_neff; i++) {
  //   printf("%d\n", i);
  // printf("%d %d\n" ,gunfires[i].start.x, gunfires[i].start.y);
  //   printf("%d %d\n" ,gunfires[i].end.x, gunfires[i].end.y);
  // }
  // printf("%d %d\n", rocket.y_borders[0], rocket.y_borders[1]);
  // printf("%d %d\n", rocket.x_borders[0], rocket.x_borders[1]);
  return 0;
}
