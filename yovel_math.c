
#include "Graphics.h"

#define EPSILON 1e-6
#define MAX_ITERATIONS 1000

typedef double (*Function)(double);

double derivative(Function f, double x){
    return (f(x + EPSILON) - f(x)) / EPSILON; 
}

double findRoot(Function f, double x0){
    double x = x0;
    for(int i = 0; i < MAX_ITERATIONS; i++){
        x = x - f(x) / derivative(f, x);
    }
    return x;
}

double f1(double x){
    return x*x*x-2*x*x + 0.25*x + 0.5;
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

    if (!Canvas_save_to_ppm(canvas, "./Graphic_output/test.ppm")) {
        fprintf(stderr, "Failed to save canvas\n");
    }

    Canvas_destroy(canvas);

    return 0;
}
