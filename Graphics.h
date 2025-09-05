#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

typedef uint32_t Color;

enum Colors {
    COLOR_BLACK   = 0x000000,
    COLOR_WHITE   = 0xFFFFFF,
    COLOR_RED     = 0xFF0000,
    COLOR_GREEN   = 0x00FF00,
    COLOR_BLUE    = 0x0000FF,
    COLOR_YELLOW  = 0xFFFF00,
    COLOR_CYAN    = 0x00FFFF,
    COLOR_MAGENTA = 0xFF00FF,
    COLOR_GRAY    = 0x808080,
    COLOR_ORANGE  = 0xFFA500,
    COLOR_PINK    = 0xFFC0CB,
    COLOR_BROWN   = 0xA52A2A,
};

typedef struct Point_t {
    size_t x;
    size_t y;
} Point;

typedef struct Canvas_t Canvas;

Canvas* Canvas_create(size_t height, size_t width);
void Canvas_destroy(Canvas* canvas);

size_t Canvas_getHeight(Canvas* canvas);
size_t Canvas_getWidth(Canvas* canvas);
Color Canvas_get_pixel(Canvas* canvas, size_t x, size_t y);
void Canvas_set_pixel(Canvas* canvas, size_t x, size_t y, Color c);
void Canvas_clear(Canvas* canvas, Color c);
// Color* Canvas_at(Canvas* canvas, size_t x, size_t y);

void Canvas_draw_rect(Canvas* canvas, Point p0, Point p1, Color c);

bool Canvas_save_to_ppm(Canvas* canvas, const char* output_name);

#endif // GRAPHICS_H
