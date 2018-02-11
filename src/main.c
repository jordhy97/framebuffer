#include "framebuffer.h"
#include "color.h"
#include "point.h"
#include "plane.h"
#include "cannonball.h"
#include "person.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <stdlib.h>

#define MAX_CANNON_BALLS 3
#define PERSON_SPEED -60

Framebuffer fb;
Plane plane;
CannonBall cannon_ball_types[3];
Person person;

void draw_plane() {
  int i;
  fb_draw_raster_polygon(&fb, &plane.body, COLOR_RED, COLOR_RED);
  for (i = 0; i < 2; i++) {
    fb_draw_raster_polygon(&fb, &plane.mirrors[i], COLOR_WHITE, COLOR_WHITE);
  }
  for (i = 0; i < 2; i++) {
    fb_draw_raster_polygon(&fb, &plane.wings[i], COLOR_RED, COLOR_RED);
  }
  for (i = 0; i < 3; i++) {
    fb_draw_raster_polygon(&fb, &plane.tails[i], COLOR_RED, COLOR_RED);
  }
  for (i = 0; i < 8; i++) {
    fb_draw_raster_polygon(&fb, &plane.propellers[i], COLOR_WHITE, COLOR_WHITE);
  }
  for (i = 0; i < 3; i++) {
    fb_draw_raster_polygon(&fb, &plane.wheel_connectors[i], COLOR_RED, COLOR_RED);
  }
  for (i = 0; i < 3; i++) {
    fb_draw_raster_polygon(&fb, &plane.wheels[i], COLOR_WHITE, COLOR_BLACK);
  }
  plane_move_wheel(&plane);
}

void draw_background() {
  int x, y;
  for (y = 0; y < fb.vinfo.yres; y++) {
    for (x = 0; x < fb.vinfo.xres; x++) {
      if ((y == fb.vinfo.yres / 2 + 120|| y == fb.vinfo.yres / 2 + 121) && x % 2 == 0) {
        fb_draw_pixel(&fb, point_create(x, y), COLOR_DARK_GREEN);
      } else if (y > fb.vinfo.yres / 2 + 121){
        fb_draw_pixel(&fb, point_create(x, y), COLOR_GREEN);
      } else {
          fb_draw_pixel(&fb, point_create(x, y), COLOR_DEEP_SKY_BLUE);
      }
    }
  }
}

void draw_person() {
  int i;

  person_move(&person);
  fb_draw_raster_polygon(&fb, &person.person, COLOR_BLACK, COLOR_BLACK);
  if (person.y_speed > 0) {
    fb_draw_raster_polygon(&fb, &person.parachute, COLOR_WHITE, COLOR_YELLOW);
    for (i = 0 ; i < 4; i++) {
      fb_draw_raster_polygon(&fb, &person.parachute_strings[i], COLOR_WHITE, COLOR_WHITE);
    }
  }
}

void flash() {
  int x, y;
  for (y = 0; y < fb.vinfo.yres; y++) {
    for (x = 0; x < fb.vinfo.xres; x++) {
      fb_draw_pixel(&fb, point_create(x, y), COLOR_WHITE);
    }
  }
  fb_display(&fb);
  usleep(1000/60 * 1000);
}

int main()
{

  CannonBall cannon_balls[MAX_CANNON_BALLS];
  int cannon_balls_neff = 0;
  int head = 0;
  int tail = -1;
  int counter = -1;
  int i, j, x, y;
  Color color;

  char c='D';
  struct termios old_tio, new_tio;


  srand(time(NULL));
  fb_init(&fb, "/dev/fb0");

  plane_init(&plane, "../data/plane.txt");
  plane_set_center(&plane, point_create(fb.vinfo.xres / 2 - 1, fb.vinfo.yres / 2 - 100));

  person_init(&person, "../data/person.txt", PERSON_SPEED);
  person_set_center(&person, point_create(fb.vinfo.xres / 2 - 1, fb.vinfo.yres / 2 - 100));

  cannon_ball_init(&cannon_ball_types[0], "../data/cannonball.txt", 5, -60);
  cannon_ball_init(&cannon_ball_types[1], "../data/cannonball.txt", 0, -60);
  cannon_ball_init(&cannon_ball_types[2], "../data/cannonball.txt", -5, -60);
  for (i = 0; i < 3; i++) {
    cannon_ball_set_center(&cannon_ball_types[i], point_create(fb.vinfo.xres / 2 - 1, fb.vinfo.yres - 1));
  }

  /* get the terminal settings for stdin */
  tcgetattr(STDIN_FILENO,&old_tio);

  /* we want to keep the old setting to restore them a the end */
  new_tio = old_tio;

  /* disable canonical mode (buffered i/o) and local echo */
  new_tio.c_lflag &=(~ICANON & ~ECHO);

  /* set the new settings immediately */
  tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);

  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking

  /* Main game loop */
  for (i = 0; i < 249; i++) {
    draw_background();

    if (plane.hit_counter > 0) {
      draw_person();
    }
    draw_plane();

    /* Read fire cannon ball command */
    read(STDIN_FILENO, &c, 1);
    if (c == ' ' && cannon_balls_neff < MAX_CANNON_BALLS) {
      tail = (tail + 1) % MAX_CANNON_BALLS;
      counter = (counter + 1) % 3;
      cannon_balls[tail] = cannon_ball_types[counter];
      c = 'D';
      cannon_balls_neff++;
    }
    tcflush(0, TCIFLUSH);

    /* Remove destroyed cannon balls */
    j = head;
    while (cannon_balls[j].destroyed == 1 && cannon_balls_neff > 0) {
      cannon_balls_neff--;
      head = (head + 1) % MAX_CANNON_BALLS;
      j = head;
    }

    /* Draw cannon balls */
    for (j = head; j < head + cannon_balls_neff; j++) {
      if (cannon_balls[j % MAX_CANNON_BALLS].destroyed == 0) {
        fb_draw_raster_polygon(&fb, &cannon_balls[j % MAX_CANNON_BALLS].body, COLOR_GRAY, COLOR_GRAY);
      }
    }

    /* Check cannon balls collision */
    for (j = head; j < head + cannon_balls_neff; j++) {
      y = cannon_balls[j % MAX_CANNON_BALLS].y_min;
      while(cannon_balls[j % MAX_CANNON_BALLS].destroyed == 0 && y < cannon_balls[j % MAX_CANNON_BALLS].y_max) {
        x = cannon_balls[j % MAX_CANNON_BALLS].x_min;
        while (cannon_balls[j % MAX_CANNON_BALLS].destroyed == 0 && x < cannon_balls[j % MAX_CANNON_BALLS].x_max) {
          color = fb_get_pixel_color(&fb, point_create(x, y));
          if (color_is_color(color, COLOR_RED) || color_is_color(color, COLOR_WHITE)) {
            cannon_balls[j % MAX_CANNON_BALLS].destroyed = 1;
            plane.hit_counter++;
            if (plane.hit_counter > 3) {
              plane.hit_counter = 3;
            }
          } else {
            x++;
          }
        }
        y++;
      }
      cannon_ball_move(&cannon_balls[j % MAX_CANNON_BALLS]);
    }

    if ((i + 1) % 50 == 0) {
      plane_scale(&plane, 2);
      if (plane.hit_counter == 0) {
        person_scale(&person, 2);
      }
    }

    plane_rotate_propellers(&plane);

    usleep(1000 /60 * 1000);
    fb_display(&fb);
  }

  flash();
  fb_clear(&fb);
  fb_display(&fb);

  tcflush(0, TCIFLUSH);

  /* restore the former settings */
  tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);

  /* Close the framebuffer */
  fb_close(&fb);

  return 0;
}
