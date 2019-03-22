#include <ctype.h>
#include <stddef.h>
#include "my_string.h"

typedef struct _vector_string{
    string* data;
    size_t* _size;
    size_t* _capacity;
    string *_begin, *_end;
} svector_t;

svector_t init_vector_s(){
    svector_t t;
    t._size = (size_t*)malloc(sizeof(size_t));
    t._capacity = (size_t*)malloc(sizeof(size_t));
    t.data = (string*)malloc(sizeof(string));
    *(t._size) = 0u;
    *(t._capacity) = 1;
    t._begin = &t.data[0];
    t._end = &t.data[0];
    return t;
}

svector_t svector_add(string s){
    
}