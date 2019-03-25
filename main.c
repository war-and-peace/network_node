#include "dbase.h"
#include <stdio.h>

dbase_t* db;
#define IP "127.0.0.1"
#define PORT "2003"
#define NAME "RA"

int initialize_database(string knode_ip, string knode_port, string knode_name){
    db = (dbase_t*)malloc(sizeof(dbase_t));
    if(db == NULL){
        perror("can't allocate memory for database");
        return errno;
    }
    db->n = 1;
    db->known_nodes = (node_t*)malloc(sizeof(node_t));
    if(db->known_nodes == NULL){
        perror("Can't allocate memory for known_nodes");
        return errno;
    }
    db->known_nodes->name = (string*)malloc(sizeof(string));
    *(db->known_nodes->name) = init_string_s(knode_name);
    db->known_nodes->ip = (string*)malloc(sizeof(string));
    *(db->known_nodes->ip) = init_string_s(knode_ip);
    db->known_nodes->port = (string*)malloc(sizeof(string));
    *(db->known_nodes->port) = init_string_s(knode_port);
    db->known_nodes->files = (svector_t*)malloc(sizeof(svector_t));
    return 0;
}

int main(){
    int ret = initialize_database(init_string_c(IP), init_string_c(PORT), init_string_c(NAME));
    if(ret != 0)return ret;
    
    return 0;
}