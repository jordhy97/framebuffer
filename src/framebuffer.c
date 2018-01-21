#include "framebuffer.h"
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

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

/* Draw a pixel with specified color to the specified point in framebuffer */
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
