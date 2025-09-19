#ifndef DARRAY_H_
#define DARRAY_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define INIT_CAPACITY 4
#define EXPANSION_RATE 2

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

typedef struct {
    const int ok;
    const int no_obj;
    const int not_sortable;
    const int out_of_memory;
    const int unknown;
} Error_t;

static const Error_t Error = {
    0, 
    1, 
    2, 
    3, 
    4,
    
};

typedef void* (*FuncCopy)(const void*);
typedef void  (*FuncDestroy)(void*);
typedef void  (*FuncPrint)(const void*, FILE*);
typedef int   (*FuncComper)(const void*, const void*);

typedef struct {
    void*       data;
    FuncCopy    copy;
    FuncDestroy destroy;
    FuncPrint   print;
    FuncComper  comper;
} Obj_t;

typedef struct Darray_t Darray;

Darray*   Darray_create();
void      Darray_destroy(Darray* darray);
Darray*   Darray_copy(Darray* darray);

size_t  Darray_getSize(const Darray* darray);
size_t  Darray_getCapacity(const Darray* darray);
void    Darray_push(Darray* darray, Obj_t obj);

void    Darray_print(const Darray* darray, FILE* file);
int     Darray_comper(const Darray* a, const Darray* b);

#endif // DARRAY_H_
