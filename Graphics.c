
#include "Graphics.h"

static void swap(size_t* a, size_t* b){
    size_t tmp = *b;
    *b = *a;
    *a = tmp;
}

struct Canvas_t {
    Color* data;
    size_t height;
    size_t width;
};

Canvas* Canvas_create(size_t height, size_t width) {
    Canvas* res = (Canvas*)malloc(sizeof(*res));
    if(!res){
        fprintf(stderr, "ERROR: failed to create canvas: %s\n", strerror(errno));
        return NULL;
    }
    res->width = width;
    res->height = height;
    res->data = calloc(width * height, sizeof(Color));
    if (!res->data) {
        fprintf(stderr, "ERROR: failed to create canvas: %s\n", strerror(errno));
        free(res);
        return NULL;
    }
    return res;
}

void Canvas_destroy(Canvas* canvas) {
    if (!canvas) return;
    free(canvas->data);
    free(canvas);
}

Color Canvas_get_pixel(Canvas* canvas, size_t x, size_t y){
    if (!canvas || !canvas->data) return 0;
    if (x >= canvas->width || y >= canvas->height) return 0; 
    return canvas->data[y * canvas->width + x];
}

void Canvas_set_pixel(Canvas* canvas, size_t x, size_t y, Color c){
    if (!canvas || !canvas->data) return;
    if (x >= canvas->width || y >= canvas->height) return; 
    canvas->data[y * canvas->width + x] = c;
}

size_t Canvas_getHeight(Canvas* canvas){
    if(!canvas || !canvas->data) return 0;
    return canvas->height;
}
size_t Canvas_getWidth(Canvas* canvas){
    if(!canvas || !canvas->data) return 0;
    return canvas->width;
}

void Canvas_clear(Canvas* canvas, Color c){
    if (!canvas || !canvas->data) return;
    for (size_t y = 0; y < canvas->height; y++) {
        for (size_t x = 0; x < canvas->height; x++) {
            Canvas_set_pixel(canvas, x, y, c);
        }
    }
}

void Canvas_fill_rect(Canvas *canvas, Rect* rect) {
    if (!canvas || !canvas->data) return;

    if (rect->top_left.y > rect->bottom_right.y) { 
        size_t tmp = rect->top_left.y; 
        rect->top_left.y = rect->bottom_right.y; 
        rect->bottom_right.y = tmp; 
    }
    if (rect->top_left.x > rect->bottom_right.x) { 
        size_t tmp = rect->top_left.x; 
        rect->top_left.x = rect->bottom_right.x; 
        rect->bottom_right.x = tmp; 
    }

    size_t start_x = (rect->top_left.x < canvas->width) ? rect->top_left.x : canvas->width - 1;
    size_t start_y = (rect->top_left.y < canvas->height) ? rect->top_left.y : canvas->height - 1;
    size_t end_x   = (rect->bottom_right.x < canvas->width) ? rect->bottom_right.x : canvas->width - 1;
    size_t end_y   = (rect->bottom_right.y < canvas->height) ? rect->bottom_right.y : canvas->height - 1;

    for (size_t y = start_y; y <= end_y; y++) {
        for (size_t x = start_x; x <= end_x; x++) {
            Canvas_set_pixel(canvas, x, y, rect->fill);
        }
    }
}

void Canvas_fill_circle(Canvas *canvas, Circle* circle)
{
    if (!canvas || !canvas->data) return;
    for (size_t y = 0; y < canvas->height; y++) {
        for (size_t x = 0; x < canvas->width; x++) {
            if((x - circle->center.x)*(x - circle->center.x) + 
                (y - circle->center.y)*(y - circle->center.y) 
                    < circle->radius*circle->radius) 
            Canvas_set_pixel(canvas, x, y, circle->fill);
        }
    }
}

void Canvas_draw_line(Canvas *canvas, Line *line) {
    if (!canvas || !canvas->data) return;

    int x0 = (int)line->start.x;
    int y0 = (int)line->start.y;
    int x1 = (int)line->end.x;
    int y1 = (int)line->end.y;
    Color c = line->color;

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        Canvas_set_pixel(canvas, x0, y0, c);

        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx)  { err += dx; y0 += sy; }
    }
}

void Canvas_outline_rect(Canvas* canvas, Rect* rect){
    if (!canvas || !canvas->data) return;

    Point p0 = {.x = rect->top_left.x, .y = rect->top_left.y};
    Point p1 = {.x = rect->bottom_right.x, .y = rect->top_left.y};
    Point p2 = {.x = rect->bottom_right.x, .y = rect->bottom_right.y};
    Point p3 = {.x = rect->top_left.x, .y = rect->bottom_right.y};

    Line line0 = {.start = p0, .end = p1, .color = rect->outline};
    Line line1 = {.start = p1, .end = p2, .color = rect->outline};
    Line line2 = {.start = p2, .end = p3, .color = rect->outline};    
    Line line3 = {.start = p3, .end = p0, .color = rect->outline};

    Canvas_draw_line(canvas, &line0);
    Canvas_draw_line(canvas, &line1);
    Canvas_draw_line(canvas, &line2);
    Canvas_draw_line(canvas, &line3);
}
void Canvas_outline_circle(Canvas* canvas, Circle* circle){
    if (!canvas || !canvas->data) return;
    size_t dr = 1;
    for (size_t y = 0; y < canvas->height; y++) {
        for (size_t x = 0; x < canvas->width; x++) {
            if((x - circle->center.x)*(x - circle->center.x) + 
                (y - circle->center.y)*(y - circle->center.y) 
                    <= (circle->radius + dr)*(circle->radius +dr)
                &&
                (x - circle->center.x)*(x - circle->center.x) + 
                (y - circle->center.y)*(y - circle->center.y) 
                    > (circle->radius)*(circle->radius)) 
            Canvas_set_pixel(canvas, x, y, circle->outline);
        }
    }
}

bool Canvas_save_to_ppm(Canvas *canvas, const char *output_name)
{
    if (!canvas || !canvas->data) {
        fprintf(stderr, "ERROR: invalid canvas\n");
        return false;
    }

    FILE *fp = fopen(output_name, "wb");
    if (!fp) {
        fprintf(stderr, "ERROR: cannot open file: `%s` due to: `%s`\n", output_name, strerror(errno));
        return false;
    }

    
    fprintf(fp, "P6\n%zu %zu\n255\n", canvas->width, canvas->height);

    for (size_t y = 0; y < canvas->height; y++) {
        for (size_t x = 0; x < canvas->width; x++) {
            uint32_t pixel = canvas->data[y * canvas->width + x];

            // Assume format 0xAARRGGBB
            // TODO: alpha not in use for now...
            unsigned char rgb[3];
            rgb[0] = (pixel >> 16) & 0xFF; // Red
            rgb[1] = (pixel >> 8) & 0xFF;  // Green
            rgb[2] = (pixel >> 0) & 0xFF;  // Blue

            if (fwrite(rgb, 1, 3, fp) != 3) {
                 fprintf(stderr, "ERROR: failed to write pixel data: %s\n", strerror(errno));
                 fclose(fp);
                 return false;
            }               
        }
    }

    fclose(fp);
    return true;
}

