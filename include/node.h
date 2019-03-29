#include "vector.h"

typedef struct _node{
    string* name;
    string* ip;
    string* port;
    svector_t* files;
} node_t;

node_t init_node();