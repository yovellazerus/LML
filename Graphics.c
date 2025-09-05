
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
    for (size_t y = 0; y <= canvas->height; y++) {
        for (size_t x = 0; x <= canvas->height; x++) {
            Canvas_set_pixel(canvas, x, y, c);
        }
    }
}

void Canvas_draw_rect(Canvas *canvas, Point p0, Point p1, Color c) {
    if (!canvas || !canvas->data) return;

    // Ensure top-left -> bottom-right
    if (p0.y > p1.y) { size_t tmp = p0.y; p0.y = p1.y; p1.y = tmp; }
    if (p0.x > p1.x) { size_t tmp = p0.x; p0.x = p1.x; p1.x = tmp; }

    // Clamp to canvas bounds
    size_t start_x = (p0.x < canvas->width) ? p0.x : canvas->width - 1;
    size_t start_y = (p0.y < canvas->height) ? p0.y : canvas->height - 1;
    size_t end_x   = (p1.x < canvas->width) ? p1.x : canvas->width - 1;
    size_t end_y   = (p1.y < canvas->height) ? p1.y : canvas->height - 1;

    for (size_t y = start_y; y <= end_y; y++) {
        for (size_t x = start_x; x <= end_x; x++) {
            Canvas_set_pixel(canvas, x, y, c);
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

    // Write header: P6, width height, max color
    fprintf(fp, "P6\n%zu %zu\n255\n", canvas->width, canvas->height);

    // Write pixel data
    for (size_t y = 0; y < canvas->height; y++) {
        for (size_t x = 0; x < canvas->width; x++) {
            uint32_t pixel = canvas->data[y * canvas->width + x];

            // Assume format 0xRRGGBB
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