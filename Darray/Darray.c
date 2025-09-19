
#include "Darray.h"

struct Darray_t {
  Obj_t* carr;
  size_t size;
  size_t capacity;
};

static void expand(Darray* darray){
    if(!darray) return;
    Obj_t* new_carr = malloc(sizeof(*new_carr) * darray->capacity * EXPANSION_RATE);
    if(!new_carr) return;
    darray->capacity = darray->capacity * EXPANSION_RATE;
    for(size_t i = 0; i < darray->size; i++){
        new_carr[i] = darray->carr[i];
    }
    free(darray->carr);
    darray->carr = new_carr;
}

static int Obj_comper(const Obj_t* a, const Obj_t* b) {
    assert(a->comper == b->comper);
    return a->comper(a->data, b->data);
}

Darray* Darray_create(){
    Darray* res = malloc(sizeof(*res));
    if(!res){
        return NULL;
    }
    Obj_t* carr = malloc(sizeof(*carr) * INIT_CAPACITY);
    if(!carr){
        free(res);
        return NULL;
    }
    res->size = 0;
    res->capacity = INIT_CAPACITY;
    res->carr = carr;
    return res;
}

void Darray_destroy(Darray* darray){
    if(!darray){
        return;
    }
    for(size_t i = 0; i < darray->size && darray->carr[i].data != NULL; i++){
        if(darray->carr[i].destroy) darray->carr[i].destroy(darray->carr[i].data);
    }
    darray->size = 0;
    darray->capacity = 0;
    darray->carr = NULL;
    free(darray);
}

void Darray_push(Darray* darray, Obj_t obj){
    if(!darray) return;
    if(darray->size == darray->capacity) expand(darray);
    darray->carr[darray->size] = obj;
    if(obj.copy) darray->carr[darray->size].data = obj.copy(obj.data);
    else darray->carr[darray->size].data = obj.data;
    darray->size++;
}

void Darray_print(const Darray* darray, FILE* file){
    if(!darray){
      fprintf(file, "null"); 
      return;
    }
    fprintf(file, "[");
    for(size_t i = 0; i < darray->size && darray->carr[i].data != NULL; i++){
        if(i != 0) fprintf(file, ", ");
        if(darray->carr[i].print) darray->carr[i].print(darray->carr[i].data, file);
        else fprintf(file, "%p", darray->carr[i].data);
    }
    fprintf(file, "]");
}

size_t  Darray_getSize(const Darray* darray){
    if(!darray) return 0;
    return darray->size;
}

size_t  Darray_getCapacity(const Darray* darray){
    if(!darray) return 0;
    return darray->capacity;
}

int Darray_sort(Darray* d) {
    if (!d) return Error.no_obj;
    
    if( d->size == 0) return Error.ok;

    FuncComper cmp = d->carr[0].comper;
    if (!cmp) return Error.not_sortable;

    for (size_t i = 1; i < d->size; i++) {
        if (d->carr[i].comper != cmp)
            return Error.not_sortable;
    }

    qsort(d->carr, d->size, sizeof(*(d->carr)), 
        (int(*)(const void*, const void*))Obj_comper);

    return Error.ok;
}
