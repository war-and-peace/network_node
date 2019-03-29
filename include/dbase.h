#include "node.h"
#include <stdio.h>

typedef struct _database{
    node_t* known_nodes;
    size_t n;
    size_t c;
} dbase_t;

extern dbase_t* db;
extern svector_t* mfiles;

int initialize_database(string knode_ip, string knode_port, string knode_name);
int database_add(node_t node);
int database_update(node_t node, svector_t files_list);
int initialize_database_clean();