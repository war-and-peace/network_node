#include "node.h"

typedef struct _database{
    node_t* known_nodes;
    size_t n;
} dbase_t;