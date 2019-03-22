#include <ctype.h>
#include <stddef.h>
#include "my_string.h"

typedef struct _vector_string{
    string* data;
    size_t* _size;
    size_t* _capacity;
    string *_begin, *_end;
} svector_t;

string sv_at(svector_t t, size_t pos);
svector_t svector_add(svector_t v, string s);
svector_t init_svector();
int can_add(size_t* size, size_t* c);