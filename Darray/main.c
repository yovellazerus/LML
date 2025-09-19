
#include <string.h>

#include "Darray.h"

void* string_copy(const void* str){
    return strdup(str);
}

void string_print(const void* str, FILE* file){
    fprintf(file, "%s", (char*)str);
}

void string_destroy(void* str){
    free(str);
}

void int_print(const void* n, FILE* file){
    fprintf(file, "%d", *(int*)n);
}

void double_print(const void* x, FILE* file){
    fprintf(file, "%lf", *(double*)x);
}

typedef struct {
    float x;
    float y;
    float z;
} Point;

void Point_print(const void* p, FILE* file){
    const Point* p_in = p;
    fprintf(file, "(%.2f, %.2f, %.2f)", p_in->x, p_in->y, p_in->z);
}

void char_print(const void* c, FILE* file){
    fprintf(file, "%c", *(char*)c);
}

void Darray_print_rawper(const void* d, FILE* file){
    Darray_print((const Darray*)d, file);
}

void Darray_destroy_rawper(void* d){
    Darray_destroy((Darray*)d);
}

int main()
{
    Darray* d = Darray_create();
    
    Darray_push(d, (Obj_t){"Hello", string_copy, string_destroy, string_print, NULL});
    int n = 42;
    Darray_push(d, (Obj_t){&n, NULL, NULL, int_print, NULL});
    double x = 3.14159265359;
    Darray_push(d, (Obj_t){&x, NULL, NULL, double_print, NULL});
    Point p = {2.2, 3.4, 5.7};
    Darray_push(d, (Obj_t){&p, NULL, NULL, Point_print, NULL});
    char c = '$';
    Darray_push(d, (Obj_t){&c, NULL, NULL, char_print, NULL});
    
    Darray* list = Darray_create();
    int n1 = 33;
    Darray_push(list, (Obj_t){&n1, NULL, NULL, int_print, NULL});
    double x1 = 2.71828182846;
    Darray_push(list, (Obj_t){&x1, NULL, NULL, double_print, NULL});
    Point p1 = {1.3, 2.9, -5.1};
    Darray_push(list ,(Obj_t){&p1, NULL, NULL, Point_print, NULL});
    Darray_push(d, (Obj_t){list, NULL, Darray_destroy_rawper, Darray_print_rawper, NULL});
    
    Darray_print(d, stdout);
    
    Darray_destroy(d);

    return 0;
}