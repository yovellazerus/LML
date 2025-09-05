
#include "Graphics.h"

double f0(double x){
    return x*x;
}

double f1(double x){
    return x*x*x-2*x*x + 0.25*x + 0.5;
}

double f2(double x){
    return pow(x, x);
}

int main() {
    Canvas* canvas = Canvas_create(200, 200);
    if (!canvas) {
        return 1;
    }

    Canvas_clear(canvas, COLOR_BLACK);

    Circle circle = {.center = {100, 100}, .radius = 25, .fill = COLOR_BLUE, .outline = COLOR_CYAN};
    Canvas_fill_circle(canvas, &circle);
    Canvas_outline_circle(canvas, &circle);

    Rect rect = {.top_left = {20, 20}, .bottom_right = {70, 60}, .fill = COLOR_RED, .outline = COLOR_MAGENTA};
    Canvas_fill_rect(canvas, &rect);
    Canvas_outline_rect(canvas, &rect);

    Line line1 = {.start = {50, 50}, .end = {150, 150}, .color = COLOR_WHITE};
    Canvas_draw_line(canvas, &line1);

    Line line2 = {.start = {30, 75}, .end = {75, 100}, .color = COLOR_WHITE};
    Canvas_draw_line(canvas, &line2);

    if (!Canvas_save_to_ppm(canvas, "./Graphic_output/basic_shapes.ppm")) {
        fprintf(stderr, "Failed to save canvas\n");
    }

    Canvas_clear(canvas, COLOR_BLACK);

    Line x_axis = {.start = {0, Canvas_getHeight(canvas) / 2}, .end = {Canvas_getWidth(canvas), Canvas_getHeight(canvas) / 2}, .color = COLOR_WHITE};
    Line y_axis = {.start = {Canvas_getWidth(canvas) / 2, 0}, .end = {Canvas_getWidth(canvas) / 2, Canvas_getHeight(canvas)}, .color = COLOR_WHITE};
    Canvas_draw_line(canvas, &x_axis);
    Canvas_draw_line(canvas, &y_axis);

    Function function0 = {.function_ptr = f0, .color = COLOR_RED, -2, 2, -2, 2};
    Function function1 = {.function_ptr = f1, .color = COLOR_GREEN, -2, 2, -2, 2};
    Function function2 = {.function_ptr = f2, .color = COLOR_CYAN, -2, 2, -2, 2};

    Canvas_draw_function(canvas, &function0);
    Canvas_draw_function(canvas, &function1);
    Canvas_draw_function(canvas, &function2);

    
    if (!Canvas_save_to_ppm(canvas, "./Graphic_output/basic_function.ppm")) {
        fprintf(stderr, "Failed to save canvas\n");
    }

    Canvas_destroy(canvas);

    return 0;
}
