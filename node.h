#include "my_string.h"
#include "vector.h"

typedef struct _node{
    string* name;
    string* ip;
    string* port;
    svector_t* files;
} node_t;