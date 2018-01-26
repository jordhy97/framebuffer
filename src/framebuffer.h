#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#define BUFFER_CAPACITY 8294400

#include <linux/fb.h>
#include <stdint.h>
#include "point.h"
#include "color.h"

typedef struct {
  int device;
  uint8_t *address; //pointer to screen memory
  long int screen_memory_size;
  uint8_t buffer[BUFFER_CAPACITY];
  struct fb_fix_screeninfo finfo;
  struct fb_var_screeninfo vinfo;
} Framebuffer;

/* Initialize the framebuffer and set color depth to 32bpp */
void fb_init(Framebuffer *fb, char *device_path);

/* Draw a pixel with specified color to the specified point in the framebuffer */
void fb_draw_pixel(Framebuffer *fb, Point position, Color color);

/* Draw a line with specified color from the specified start and end point
in the framebuffer */
void fb_draw_line(Framebuffer *fb, Point start, Point end, Color color);

/* Draw a line with specified color from the specified start and end point
with low gradient (0 < m < 1 or -1 < m < 0) in the framebuffer using Bresenham algorithm */
void fb_draw_line_low(Framebuffer *fb, Point start, Point end, Color color);

/* Draw a line with specified color from the specified start and end point
with steep gradient (> 1 or < -1) in the framebuffer using Bresenham algorithm */
void fb_draw_line_high(Framebuffer *fb, Point start, Point end, Color color);

/* Clear the framebuffer */
void fb_clear(Framebuffer *fb);

/* Display the framebuffer */
void fb_display(Framebuffer *fb);

/* Close the framebuffer */
void fb_close(Framebuffer *fb);

#endif
