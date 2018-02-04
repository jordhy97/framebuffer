#include "polygon.h"
#include <stdlib.h>

/* Initialize a polygon */
void polygon_init(Polygon *polygon, Color border_color, Color fill_color) {
  int i, j, k, temp, prev, next;
  Intersections intersections;

  for (i = 0; i < polygon->height; i++) {
    for (j = 0; j < polygon->width; j++) {
      polygon->canvas[i][j] = NO_FILL;
    }
    intersections.neff[i] = 0;
  }

  for (i = 0; i < polygon->neff; i++) {
    prev = i - 1;
    next = i + 1;
    if (prev < 0) {
      prev = polygon->neff - 1;
    }
    if (next == polygon->neff) {
      next = 0;
    }
    if ((polygon->points[i].y > polygon->points[next].y && polygon->points[i].y < polygon->points[prev].y) ||
        (polygon->points[i].y < polygon->points[next].y && polygon->points[i].y > polygon->points[prev].y)) {
          intersections.intersections[polygon->points[i].y][intersections.neff[polygon->points[i].y]] = polygon->points[i].x;
          intersections.neff[polygon->points[i].y]++;
    }
    polygon_draw_line(polygon, polygon->points[i], polygon->points[next], &intersections, border_color);
    if (polygon->points[i].y == polygon->points[next].y) {
      if (polygon->points[i].y < polygon->points[prev].y) {
        intersections.intersections[polygon->points[i].y][intersections.neff[polygon->points[i].y]] = polygon->points[i].x;
        intersections.neff[polygon->points[i].y]++;
      }
      if (polygon->points[next].y < polygon->points[(next + 1) % polygon->neff].y) {
        intersections.intersections[polygon->points[next].y][intersections.neff[polygon->points[next].y]] = polygon->points[next].x;
        intersections.neff[polygon->points[next].y]++;
      }
    }
  }

  for (i = 1; i < polygon->height - 1; i++) {
		for (j = 0; j < intersections.neff[i] - 1; j++) {
			for (k = 0; k < intersections.neff[i] - j - 1; k++) {
					if (intersections.intersections[i][k] > intersections.intersections[i][k + 1]) {
						temp = intersections.intersections[i][k];
						intersections.intersections[i][k] = intersections.intersections[i][k + 1];
						intersections.intersections[i][k + 1] = temp;
					}
			}
		}
	}

  for (i = 1; i < polygon->height - 1; i++) {
		if (intersections.neff[i] > 1) {
			for (j = 0; j < intersections.neff[i] - 1; j += 2) {
				for (k = intersections.intersections[i][j] + 1; k < intersections.intersections[i][j + 1]; k++) {
					polygon->canvas[i][k] = fill_color;
				}
			}
		}
	}

  for (i = 0; i < polygon->neff; i++) {
    next = i + 1;
    if (next == polygon->neff) {
      next = 0;
    }
		  polygon_draw_line(polygon, polygon->points[i], polygon->points[next], &intersections, border_color);
	}
}

/* Draw polygon line */
void polygon_draw_line(Polygon *polygon, Point start, Point end, Intersections *intersections, Color color) {
  int x, y;
  if (start.x == end.x) {
    x = start.x;
    if (start.y < end.y) {
      for (y = start.y; y <= end.y; y++) {
        polygon->canvas[y][x] = color;
        intersections->intersections[y][intersections->neff[y]] = x;
        intersections->neff[y]++;
      }
    } else {
      for (y = end.y; y <= start.y; y++) {
        polygon->canvas[y][x] = color;
        intersections->intersections[y][intersections->neff[y]] = x;
        intersections->neff[y]++;
      }
    }
  } else if (start.y == end.y) {
    y = start.y;
    if (start.x < end.x) {
      intersections->intersections[y][intersections->neff[y]] = start.x;
      intersections->neff[y]++;
      intersections->intersections[y][intersections->neff[y]] = end.x;
      intersections->neff[y]++;
      for (x = start.x; x <= end.x; x++) {
        polygon->canvas[y][x] = color;
      }
    } else {
      intersections->intersections[y][intersections->neff[y]] = end.x;
      intersections->neff[y]++;
      intersections->intersections[y][intersections->neff[y]] = start.x;
      intersections->neff[y]++;
      for (x = end.x; x <= start.x; x++) {
        polygon->canvas[y][x] = color;
      }
    }
  } else if (abs(end.y - start.y) < abs(end.x - start.x)) {
    if (start.x > end.x) {
      polygon_draw_line_low(polygon, end, start, intersections, color);
    } else {
      polygon_draw_line_low(polygon, start, end, intersections, color);
    }
  } else {
    if (start.y > end.y) {
      polygon_draw_line_high(polygon, end, start, intersections, color);
    } else {
      polygon_draw_line_high(polygon, start, end, intersections, color);
    }
  }
}

/* Draw polygon line */
void polygon_draw_line_low(Polygon *polygon, Point start, Point end, Intersections *intersections, Color color) {
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
    polygon->canvas[y][x] = color;
    if (p > 0) {
      intersections->intersections[y][intersections->neff[y]] = x;
      intersections->neff[y]++;
      y += yi;
      p -= (2 * dx);
    }
    p += (2 * dy);
  }
}

/* Draw polygon line */
void polygon_draw_line_high(Polygon *polygon, Point start, Point end, Intersections *intersections, Color color) {
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
    polygon->canvas[y][x] = color;
    intersections->intersections[y][intersections->neff[y]] = x;
    intersections->neff[y]++;
    if (p > 0) {
      x += xi;
      p -= (2 * dy);
    }
    p += (2 * dx);
  }
}
