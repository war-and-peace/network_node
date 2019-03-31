#include "node.h"
#include <stdio.h>

#define SIZE 1000

typedef struct _database{
    node_t* known_nodes;
    size_t n;
    size_t c;
} dbase_t;

typedef struct _inter_cdb_type{
    unsigned long id;
    unsigned long count;
} icdb_t;

typedef struct _cdatabase{
    size_t n;
    icdb_t a[SIZE];
} cdb_t;

typedef struct _bldatabase{
    unsigned long id[SIZE];
    size_t n;
} bldb_t;

extern dbase_t* db;
extern svector_t* mfiles;
extern cdb_t* cdb;
extern bldb_t* b_list;

int initialize_database(string knode_ip, string knode_port, string knode_name);
int database_add(node_t node);
int database_update(node_t node, svector_t files_list);
int initialize_database_clean();

int cdatabase_init();
int cdatabase_add(unsigned int id);
int cdatabase_has(unsigned int id);
size_t cdatabase_count(unsigned int id);
int cdatabase_clear(unsigned int id);
int cdatabase_increase(unsigned int id);
int cdatabase_decrese(unsigned int id);

int blist_init();
int blist_add(unsigned int id);
int blist_has(unsigned int id);