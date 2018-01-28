#include "framebuffer.h"
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Initialize framebuffer and set color depth to 32bpp */
void fb_init(Framebuffer *fb, char *device_path) {

  // Open framebuffer device for reading and writing
	fb->device = open(device_path, O_RDWR);
  if (fb->device == -1) {
    perror("Error: cannot open framebuffer device");
    exit(1);
  }

  // Read fixed screen information
  if (ioctl(fb->device, FBIOGET_FSCREENINFO, &fb->finfo) == -1) {
    perror("Error: failed to read fixed screen information");
    exit(2);
  }

	// Read and write settings to variable screen information
	if (ioctl(fb->device, FBIOGET_VSCREENINFO, &fb->vinfo) == -1) {
    perror("Error: failed to read variable screen information");
    exit(3);
  }
	fb->vinfo.grayscale = 0;
	fb->vinfo.bits_per_pixel = 32;
	fb->vinfo.xoffset = 0;
	fb->vinfo.yoffset = 0;
	if (ioctl(fb->device, FBIOPUT_VSCREENINFO, &fb->vinfo) == -1) {
    perror("Error: failed to write variable screen information");
    exit(4);

  }

  // Calculate screen memory size in bytes
	fb->screen_memory_size = fb->vinfo.yres_virtual * fb->finfo.line_length;

  // Map the device to memory
  fb->address = mmap(0, fb->screen_memory_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb->device, 0);
  if (*(fb->address) == -1) {
    perror("Error: failed to map framebuffer device to memory");
    exit(5);
  }
}

/* Draw a pixel with specified color to the specified point in the framebuffer */
void fb_draw_pixel(Framebuffer *fb, Point position, Color color) {
  long int address_offset;
  if (position.x >= 0 && position.x < fb->vinfo.xres && position.y >= 0 && position.y < fb->vinfo.yres) {
    address_offset = position.x * (fb->vinfo.bits_per_pixel/8) + position.y * fb->finfo.line_length;
		fb->buffer[address_offset] = color.b;
		fb->buffer[address_offset + 1] = color.g;
		fb->buffer[address_offset + 2] = color.r;
		fb->buffer[address_offset + 3] = 0;
  }
}

/* Draw a line with specified color from the specified start and end point
in the framebuffer */
void fb_draw_line(Framebuffer *fb, Point start, Point end, Color color) {
	int x, y;
	if (start.x == end.x) {
		x = start.x;
		if (start.y < end.y) {
			for (y = start.y; y <= end.y; y++) {
				fb_draw_pixel(fb, point_create(x, y), color);
			}
		} else {
			for (y = end.y; y <= start.y; y++) {
				fb_draw_pixel(fb, point_create(x, y), color);
			}
		}
	} else if (start.y == end.y) {
		y = start.y;
		if (start.x < end.x) {
			for (x = start.x; x <= end.x; x++) {
				fb_draw_pixel(fb, point_create(x, y), color);
			}
		} else {
			for (x = end.x; x <= start.x; x++) {
				fb_draw_pixel(fb, point_create(x, y), color);
			}
		}
	} else if (abs(end.y - start.y) < abs(end.x - start.x)) {
		if (start.x > end.x) {
			fb_draw_line_low(fb, end, start, color);
		} else {
			fb_draw_line_low(fb, start, end, color);
		}
	} else {
		if (start.y > end.y) {
			fb_draw_line_high(fb, end, start, color);
		} else {
			fb_draw_line_high(fb, start, end, color);
		}
	}
}

/* Draw a line with specified color from the specified start and end point
with low gradient (0 < m < 1 or -1 < m < 0) in the framebuffer using Bresenham algorithm */
void fb_draw_line_low(Framebuffer *fb, Point start, Point end, Color color){
	int dx, dy, p, x, y, yi;

	dx = end.x - start.x;
	dy = end.y - start.y;
	if (dy < 0) {
		yi = -1;
		dy = -dy;
	} else {
		yi = 1;
	}
	p = 2 * dy - dx;
	y = start.y;

	for (x = start.x; x <= end.x; x++) {
		fb_draw_pixel(fb, point_create(x, y), color);
		if (p > 0) {
			y += yi;
			p -= (2 * dx);
		}
		p += (2 * dy);
	}
}

/* Draw a line with specified color from the specified start and end point
with steep gradient (> 1 or < -1) in the framebuffer using Bresenham algorithm */
void fb_draw_line_high(Framebuffer *fb, Point start, Point end, Color color) {
	int dx, dy, p, x, y, xi;

	dx = end.x - start.x;
	dy = end.y - start.y;
	if (dx < 0) {
		xi = -1;
		dx = -dx;
	} else {
		xi = 1;
	}
	p = 2 * dx - dy;
	x = start.x;

	for (y = start.y; y <= end.y; y++) {
		fb_draw_pixel(fb, point_create(x, y), color);
		if (p > 0) {
			x += xi;
			p -= (2 * dy);
		}
		p += (2 * dx);
	}
}

/* Draw a dotted line with specified color and interval from the specified
start and end point in the framebuffer */
void fb_draw__dotted_line(Framebuffer *fb, Point start, Point end, int interval, Color color) {
	int x, y;
	int draw = -1;

	if (start.x == end.x) {
		x = start.x;
		if (start.y < end.y) {
			for (y = start.y; y <= end.y; y++) {
				if ((y - start.y) % interval == 0) {
					draw = -draw;
				}
				if (draw == 1) {
					fb_draw_pixel(fb, point_create(x, y), color);
				}
			}
		} else {
			for (y = end.y; y <= start.y; y++) {
				if ((y - end.y) % interval == 0) {
					draw = -draw;
				}
				if (draw == 1) {
					fb_draw_pixel(fb, point_create(x, y), color);
				}
			}
		}
	} else if (start.y == end.y) {
		y = start.y;
		if (start.x < end.x) {
			for (x = start.x; x <= end.x; x++) {
				if ((x - start.x) % interval == 0) {
					draw = -draw;
				}
				if (draw == 1) {
					fb_draw_pixel(fb, point_create(x, y), color);
				}
			}
		} else {
			for (x = end.x; x <= start.x; x++) {
				if ((x - end.x) % interval == 0) {
					draw = -draw;
				}
				if (draw == 1) {
					fb_draw_pixel(fb, point_create(x, y), color);
				}
			}
		}
	} else if (abs(end.y - start.y) < abs(end.x - start.x)) {
		if (start.x > end.x) {
			fb_draw_dotted_line_low(fb, end, start, interval, color);
		} else {
			fb_draw_dotted_line_low(fb, start, end, interval, color);
		}
	} else {
		if (start.y > end.y) {
			fb_draw_dotted_line_high(fb, end, start, interval, color);
		} else {
			fb_draw_dotted_line_high(fb, start, end, interval, color);
		}
	}
}

/* Draw a dotted line with specified color and interval from the specified start and end point
with low gradient (0 < m < 1 or -1 < m < 0) in the framebuffer using Bresenham algorithm */
void fb_draw_dotted_line_low(Framebuffer *fb, Point start, Point end, int interval, Color color) {
	int dx, dy, p, x, y, yi;
	int draw = -1;

	dx = end.x - start.x;
	dy = end.y - start.y;
	if (dy < 0) {
		yi = -1;
		dy = -dy;
	} else {
		yi = 1;
	}
	p = 2 * dy - dx;
	y = start.y;

	for (x = start.x; x <= end.x; x++) {
		if ((x - start.x) % interval == 0) {
			draw = -draw;
		}
		if (draw == 1) {
			fb_draw_pixel(fb, point_create(x, y), color);
		}
		if (p > 0) {
			y += yi;
			p -= (2 * dx);
		}
		p += (2 * dy);
	}
}

/* Draw a dotted line with specified color and interval from the specified start and end point
with steep gradient (> 1 or < -1) in the framebuffer using Bresenham algorithm */
void fb_draw_dotted_line_high(Framebuffer *fb, Point start, Point end, int interval, Color color) {
	int dx, dy, p, x, y, xi;
	int draw = -1;

	dx = end.x - start.x;
	dy = end.y - start.y;
	if (dx < 0) {
		xi = -1;
		dx = -dx;
	} else {
		xi = 1;
	}
	p = 2 * dx - dy;
	x = start.x;

	for (y = start.y; y <= end.y; y++) {
		if ((y - start.y) % interval == 0) {
			draw = -draw;
		}
		if (draw == 1) {
			fb_draw_pixel(fb, point_create(x, y), color);
		}
		if (p > 0) {
			x += xi;
			p -= (2 * dy);
		}
		p += (2 * dx);
	}
}

/* Clear the framebuffer */
void fb_clear(Framebuffer *fb) {
  int x, y;
  for (y = 0; y < fb->vinfo.yres; y++) {
    for (x = 0; x < fb->vinfo.xres; x++) {
      fb_draw_pixel(fb, point_create(x, y), COLOR_BLACK);
    }
  }
}

/* Display the framebuffer */
void fb_display(Framebuffer *fb) {
	memcpy(fb->address, &fb->buffer, fb->screen_memory_size);
}

/* Close the framebuffer */
void fb_close(Framebuffer *fb) {
  munmap(fb->address, fb->screen_memory_size);
  close(fb->device);
}
