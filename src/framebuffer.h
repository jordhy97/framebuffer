#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#define BUFFER_CAPACITY 8294400

#include <linux/fb.h>
#include <stdint.h>
#include "point.h"
#include "color.h"
#include "polygon.h"

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

/* Draw a dotted line with specified color and interval from the specified
start and end point in the framebuffer */
void fb_draw__dotted_line(Framebuffer *fb, Point start, Point end, int interval, Color color);

/* Draw a dotted line with specified color and interval from the specified start and end point
with low gradient (0 < m < 1 or -1 < m < 0) in the framebuffer using Bresenham algorithm */
void fb_draw_dotted_line_low(Framebuffer *fb, Point start, Point end, int interval, Color color);

/* Draw a dotted line with specified color and interval from the specified start and end point
with steep gradient (> 1 or < -1) in the framebuffer using Bresenham algorithm */
void fb_draw_dotted_line_high(Framebuffer *fb, Point start, Point end, int interval, Color color);

/* Draw a polygon to the framebuffer from the given polygon */
void fb_draw_polygon(Framebuffer *fb, Polygon *polygon, Color color);

/* Draw a raster polygon to the framebuffer from the given polygon */
void fb_draw_raster_polygon(Framebuffer *fb, Polygon *polygon, Color border_color, Color fill_color);

/* Draw a raster polygon line to the framebuffer from the given start and end point */
void fb_draw_raster_polygon_line(Framebuffer *fb, Point start, Point end, Intersections *intersections, int ymin);

/* Draw a raster polygon line with low gradient (0 < m < 1 or -1 < m < 0)
to the framebuffer from the given start and end point */
void fb_draw_raster_polygon_line_low(Framebuffer *fb, Point start, Point end, Intersections *intersections, int ymin);

/* Draw a raster polygon line with steep gradient (> 1 or < -1)
to the framebuffer from the given start and end point */
void fb_draw_raster_polygon_line_high(Framebuffer *fb, Point start, Point end, Intersections *intersections, int ymin);

/* Clear the framebuffer */
void fb_clear(Framebuffer *fb);

/* Display the framebuffer */
void fb_display(Framebuffer *fb);

/* Close the framebuffer */
void fb_close(Framebuffer *fb);

/* Get pixel color of the specified point */
Color fb_get_pixel_color(Framebuffer *fb, Point position);
#endif
