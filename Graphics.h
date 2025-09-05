#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <math.h>

typedef uint32_t Color;

enum Colors {
    COLOR_BLACK   = 0x00000000,
    COLOR_WHITE   = 0x00FFFFFF,
    COLOR_RED     = 0x00FF0000,
    COLOR_GREEN   = 0x0000FF00,
    COLOR_BLUE    = 0x000000FF,
    COLOR_YELLOW  = 0x00FFFF00,
    COLOR_CYAN    = 0x0000FFFF,
    COLOR_MAGENTA = 0x00FF00FF,
    COLOR_GRAY    = 0x00808080,
    COLOR_ORANGE  = 0x00FFA500,
    COLOR_PINK    = 0x00FFC0CB,
    COLOR_BROWN   = 0x00A52A2A,
};

typedef struct Point_t {
    size_t x;
    size_t y;
} Point;

typedef struct Line_t {
    Point start;
    Point end;
    Color color;
} Line;

typedef struct Rect_t {
    Point top_left;
    Point bottom_right;
    Color fill;
    Color outline;
} Rect;

typedef struct Circle_t {
    Point center;
    size_t radius;
    Color fill;
    Color outline;
} Circle;

typedef struct Function_t {
    double (*function_ptr)(double);
    Color color;
    double x0; 
    double x1; 
    double y0; 
    double y1;
} Function;

typedef struct Canvas_t Canvas;

Canvas* Canvas_create(size_t height, size_t width);
void Canvas_destroy(Canvas* canvas);

size_t Canvas_getHeight(Canvas* canvas);
size_t Canvas_getWidth(Canvas* canvas);
Color Canvas_get_pixel(Canvas* canvas, size_t x, size_t y);
void Canvas_set_pixel(Canvas* canvas, size_t x, size_t y, Color c);
void Canvas_clear(Canvas* canvas, Color c);

void Canvas_fill_rect(Canvas* canvas, Rect* rect);
void Canvas_fill_circle(Canvas* canvas, Circle* circle);
void Canvas_outline_rect(Canvas* canvas, Rect* rect);
void Canvas_outline_circle(Canvas* canvas, Circle* circle);
void Canvas_draw_line(Canvas* canvas, Line* line);
void Canvas_draw_function(Canvas* canvas, Function* function);

bool Canvas_save_to_ppm(Canvas* canvas, const char* output_name);

#endif // GRAPHICS_H

