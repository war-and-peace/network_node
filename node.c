#include "node.h"
#include <stdio.h>

void init_node(){
  node_t node;
  node.name = (string*)malloc(sizeof(string));
  node.port = (string*)malloc(sizeof(string));
  node.ip = (string*)malloc(sizeof(string));
  
}