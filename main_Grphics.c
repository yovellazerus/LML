
#include "Graphics.h"
#include "ansi_cods.h"

double f0(double x){
    return x*x;
}

double f1(double x){
    return x*x*x-2*x*x + 0.25*x + 0.5;
}

double f2(double x){
    return pow(x, x);
}

bool basic_shapes(const char** test_name){
    *test_name = __func__;
    Canvas* canvas = Canvas_create(200, 200);
    if (!canvas) {
        return false;
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

    char path[32] = {'\0'};
    sprintf(path, "./Graphic_output/%s.ppm", *test_name);

    if (!Canvas_save_to_ppm(canvas, path)) {
        fprintf(stderr, "Failed to save canvas\n");
        Canvas_destroy(canvas);
        return false;
    }

    Canvas_destroy(canvas);

    return true;
}

bool basic_functions(const char** test_name){
    *test_name = __func__;
    Canvas* canvas = Canvas_create(200, 200);
    if (!canvas) {
        return false;
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
    
    char path[32] = {'\0'};
    sprintf(path, "./Graphic_output/%s.ppm", *test_name);

    if (!Canvas_save_to_ppm(canvas, path)) {
        fprintf(stderr, "Failed to save canvas\n");
        Canvas_destroy(canvas);
        return false;
    }

    Canvas_destroy(canvas);

    return true;
}

bool mandelbrot_set(const char** test_name){
    *test_name = __func__;
    Canvas* canvas = Canvas_create(256, 256);
    if (!canvas) {
        return false;
    }
    Canvas_clear(canvas, COLOR_BLACK);

    const size_t max_iteration = 100;

    // init condition for mandelbrot set: z0 = 0
    double x0 = 0.0;
    double y0 = 0.0;

    double x_delta = 4.0 / Canvas_getWidth(canvas);
    double y_delta = 4.0 / Canvas_getHeight(canvas);
    for(size_t a = 0; a < Canvas_getWidth(canvas); a++){
        for(size_t b = 0; b < Canvas_getHeight(canvas); b++){
            // c = a + ib -> x + iy
            // Coordinate translation:
            double x = -2.0 + x_delta*a;
            double y = 2.0 - y_delta*b;
            // z = z^2 + c
            // z^2 = (x_old + y_old*i)*(x_old + y_old*i) + x + y*i = 
            // = x_old^2 + 2*x_old*y_old*i - y_old^2 + x + y*i = 
            // x_new = x_old^2 - y_old^2 + x
            // y_new = 2*x_old*y_old*i + y*i
            double xn = x;
            double yn = y;
            size_t n = 0;
            for(n = 0; n < max_iteration; n++)
            {
                double xn_next = xn*xn - yn*yn + x;
                double yn_next = 2*xn*yn + y;

                xn = xn_next;
                yn = yn_next;

                if(xn*xn + yn*yn > 4.0) break;
            }

            Color color;
            if (n == max_iteration) {
                color = COLOR_BLACK; // inside the set
            } else {
                // magic color RGB gradient...
                double t = (double)n / max_iteration;
                uint8_t r = (uint8_t)(9  * (1 - t) * t * t * t * 255);
                uint8_t g = (uint8_t)(15 * (1 - t) * (1 - t) * t * t * 255);
                uint8_t b = (uint8_t)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
                color = (r << 16) | (g << 8) | (b << 0);
            }

            Canvas_set_pixel(canvas, a, b, color);
        }
    }

    char path[32] = {'\0'};
    sprintf(path, "./Graphic_output/%s.ppm", *test_name);

    if (!Canvas_save_to_ppm(canvas, path)) {
        fprintf(stderr, "Failed to save canvas\n");
        Canvas_destroy(canvas);
        return false;
    }

    Canvas_destroy(canvas);

    return true;
}

bool rule110(const char** test_name){
    *test_name = __func__;
    Canvas* canvas = Canvas_create(256, 256*2);
    if (!canvas) {
        return false;
    }

    const Color alive = COLOR_WHITE;
    const Color dead = COLOR_BLACK;

    Canvas_clear(canvas, dead);

    size_t rule110_pattern[8] = {0, 1, 1, 1, 0, 1, 1, 0};

    Color old_row[Canvas_getWidth(canvas)];
    Color new_row[Canvas_getWidth(canvas)];
    
    for(size_t j = 0; j < Canvas_getWidth(canvas); j++){
            old_row[j] = dead;
    }
    old_row[Canvas_getWidth(canvas) / 2] = alive;

    for(size_t i = 0; i < Canvas_getHeight(canvas); i++){
        for(size_t j = 0; j < Canvas_getWidth(canvas); j++){
            Canvas_set_pixel(canvas, j, i, old_row[j]);
        }
        for(size_t j = 1; j < Canvas_getWidth(canvas) - 1; j++){
            size_t index = ((size_t)(old_row[j - 1] / alive) << 2) | ((size_t)(old_row[j] / alive) << 1) | ((size_t)(old_row[j + 1] / alive) << 0);
            new_row[j] = alive * rule110_pattern[index];
        }
        for(size_t j = 0; j < Canvas_getWidth(canvas); j++){
            old_row[j] = new_row[j];
        }
    }

    char path[32] = {'\0'};
    sprintf(path, "./Graphic_output/%s.ppm", *test_name);

    if (!Canvas_save_to_ppm(canvas, path)) {
        fprintf(stderr, "Failed to save canvas\n");
        Canvas_destroy(canvas);
        return false;
    }

    Canvas_destroy(canvas);

    return true;
}

static size_t* generate_rule(uint8_t rule) {
    size_t* arr = malloc(8 * sizeof(size_t));
    if (!arr) {
        return NULL;
    }

    for (int i = 0; i < 8; i++) {
        arr[i] = (rule >> i) & 1;
    }

    return arr;
}

bool all_rules(const char** test_name){
    *test_name = __func__;
    
    const Color alive = COLOR_WHITE;
    const Color dead = COLOR_BLACK;

    size_t* all_rules_patterns[256] = {NULL};

    for(int rule = 0; rule < 256; rule++){
        all_rules_patterns[rule] = generate_rule(rule);
        if(!all_rules_patterns[rule]){
            for(int i = rule; i >= 0; i--){
                free(all_rules_patterns[i]);
            }
            return false;
        }
    }

    for(int rule = 0; rule < 256; rule++){
        Canvas* canvas = Canvas_create(256, 256);
        if (!canvas) {
            return false;
        }
        Canvas_clear(canvas, dead);

        Color old_row[Canvas_getWidth(canvas)];
        Color new_row[Canvas_getWidth(canvas)];
        
        for(size_t j = 0; j < Canvas_getWidth(canvas); j++){
                old_row[j] = dead;
        }
        old_row[Canvas_getWidth(canvas) / 2] = alive;

        for(size_t i = 0; i < Canvas_getHeight(canvas); i++){
            for(size_t j = 0; j < Canvas_getWidth(canvas); j++){
                Canvas_set_pixel(canvas, j, i, old_row[j]);
            }
            for(size_t j = 1; j < Canvas_getWidth(canvas) - 1; j++){
                size_t index = ((size_t)(old_row[j - 1] / alive) << 2) | ((size_t)(old_row[j] / alive) << 1) | ((size_t)(old_row[j + 1] / alive) << 0);
                new_row[j] = alive * all_rules_patterns[rule][index];
            }
            for(size_t j = 0; j < Canvas_getWidth(canvas); j++){
                old_row[j] = new_row[j];
            }
        }

        char path[32] = {'\0'};
        sprintf(path, "./rules_output/rule%zu.ppm", rule);

        if (!Canvas_save_to_ppm(canvas, path)) {
            fprintf(stderr, "Failed to save canvas\n");
            Canvas_destroy(canvas);
            return false;
        }

        Canvas_destroy(canvas);
    }

    for(int rule = 0; rule < 256; rule++){
        free(all_rules_patterns[rule]);
    }

    return true;
}

typedef bool (*Graphic_test)(const char**);

Graphic_test tests[] = {basic_shapes, basic_functions, mandelbrot_set, rule110, all_rules, NULL};

int main() {
    size_t i = 0;
    bool status = true;
    const char* test_name = NULL;
    while(tests[i]){
        status = tests[i](&test_name);
        if(status) printf(GREEN "TEST: %zu `%s` is a SUCCESS\n" RESET, i, test_name);
        else printf(RED "TEST: %zu `%s` is a FAILURE\n" RESET, i, test_name);
        i++;
    }
    return 0;
}
