
#include "Graphics.h"
#include <math.h>

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
    // Create canvas
    Canvas* canvas = Canvas_create(100, 100);
    if (!canvas) {
        return 1;
    }

    // Fill canvas with black
    Canvas_clear(canvas, COLOR_BLACK);

    // Draw a red rectangle
    Point top_left = {20, 20};
    Point bottom_right = {70, 60};
    Canvas_draw_rect(canvas, top_left, bottom_right, COLOR_RED);

    // Draw a white pixel in the center
    Canvas_set_pixel(canvas, Canvas_getWidth(canvas)/2, Canvas_getHeight(canvas)/2, COLOR_WHITE);

    // Save to PPM
    if (!Canvas_save_to_ppm(canvas, "test.ppm")) {
        fprintf(stderr, "Failed to save canvas\n");
    }

    // Free memory
    Canvas_destroy(canvas);

    return 0;
}
