#include "node.h"
#include <stdio.h>

node_t init_node(){
    node_t node;
    node.name = (string*)malloc(sizeof(string));
    node.port = (string*)malloc(sizeof(string));
    node.ip = (string*)malloc(sizeof(string));
    node.files = (svector_t*)malloc(sizeof(svector_t));
    *(node.files) = init_svector();
    return node;
}