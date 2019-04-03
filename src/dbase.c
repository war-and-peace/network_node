#include "dbase.h"

void assign_node(node_t* a, node_t* b){
    a->name = (string*)malloc(sizeof(string));
    *(a->name) = init_string_s(*(b->name));
    a->port = (string*)malloc(sizeof(string));
    *(a->port) = init_string_s(*(b->port));
    a->ip = (string*)malloc(sizeof(string));
    *(a->ip) = init_string_s(*(b->ip));
    a->files = (svector_t*)malloc(sizeof(svector_t));
    *(a->files) = init_svector();
    for(size_t i = 0;i < *((*(b->files))._size);i ++){
        *(a->files) = svector_add(*(a->files), b->files->data[i]);
    }
}

int initialize_database(string knode_ip, string knode_port, string knode_name){
    db = (dbase_t*)malloc(sizeof(dbase_t));
    if(db == NULL){
        perror("can't allocate memory for database");
        return errno;
    }

    db->n = 1;
    db->c = 1;
    
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
    *(db->known_nodes->files) = init_svector();
    return 0;
}

int initialize_database_clean(){
    db = (dbase_t*)malloc(sizeof(dbase_t));
    if(db == NULL){
        perror("can't allocate memory for database");
        return errno;
    }

    db->n = 0;
    db->c = 1;
    
    db->known_nodes = (node_t*)malloc(sizeof(node_t));
    if(db->known_nodes == NULL){
        perror("Can't allocate memory for known_nodes");
        return errno;
    }

    // db->known_nodes->name = (string*)malloc(sizeof(string));
    // *(db->known_nodes->name) = init_string_s(knode_name);
    
    // db->known_nodes->ip = (string*)malloc(sizeof(string));
    // *(db->known_nodes->ip) = init_string_s(knode_ip);
    
    // db->known_nodes->port = (string*)malloc(sizeof(string));
    // *(db->known_nodes->port) = init_string_s(knode_port);
    
    // db->known_nodes->files = (svector_t*)malloc(sizeof(svector_t));
    // *(db->known_nodes->files) = init_svector();
    return 0;
}

int database_add(node_t node){
    int flag = 0;
    // sprintln(*(node.ip));
    // sprintln(*(node.port));
    for(size_t i = 0;i < db->n;i ++){
        if(
            // equal(*(db->known_nodes->name), *(node.name)) &&
            equal(*(db->known_nodes[i].ip), *(node.ip)) &&
            equal(*(db->known_nodes[i].port), *(node.port))
        ){
            flag = 1;
            break;
        }
    }
    // printf("flag: %d\n", flag);
    if(!flag){
        if((db->c) > (db->n)){
            assign_node(&(db->known_nodes[(db->n)]), &node);
            (db->n) ++;
        }else{
            size_t c = db->c;
            c = c * 2;
            size_t n = db->n;
            node_t* temp = db->known_nodes;
            db->known_nodes = (node_t*)malloc(c * sizeof(node_t));
            for(size_t i = 0;i < n;i ++){
                assign_node(&(db->known_nodes[i]), &(temp[i]));
            }
            assign_node(&(db->known_nodes[n]), &node);
            n ++;
            db->c = c;
            db->n = n;
        }
    }
    return 0;
}

int database_update(node_t node, svector_t files_list){
    int flag = 0, index = 0;
    for(size_t i = 0;i < db->n;i ++){
        if(
            // equal(*(db->known_nodes->name), *(node.name)) &&
            equal(*(db->known_nodes[i].ip), *(node.ip)) &&
            equal(*(db->known_nodes[i].port), *(node.port))
        ){
            flag = 1;
            index = i;
            break;
        }
        // printf("step: %d\n", i);
    }
    // perror("here");
    // printf("index: %d\n", index);
    if(flag){
        for(size_t i = 0;i < *(files_list._size);i ++){
            // perror("inside loop");
            // printf("Name of node: "); sprintln(*(db->known_nodes[index].name));
            // sprintln(*(db->known_nodes[index].port));
            if(!svector_contains(*(db->known_nodes[index].files), files_list.data[i])){
                *(db->known_nodes[index].files) = svector_add(*(db->known_nodes[index].files), files_list.data[i]);
            }
        }
    }
    // perror("DONE update");
    return 0;
}

int cdatabase_init(){
    cdb = (cdb_t*)malloc(sizeof(cdb_t));
    if (cdb == NULL) return 1;
    cdb->n = 0;
    return 0;
}

int cdatabase_has(unsigned int id){
    for (size_t i = 0;i < (cdb->n);i ++){
        if(cdb->a[i].id == id){
            return 1;
        }
    }
    return 0;
}

int cdatabase_add(unsigned int id){
    size_t n = cdb->n;
    cdb->a[n].id = id;
    cdb->a[n].count = 1;
    cdb->n = n + 1;
    return 1;
}

size_t cdatabase_count(unsigned int id){
    for (size_t i = 0; i < (cdb->n);i ++){
        if(cdb->a[i].id == id){
            return cdb->a[i].count;
        }
    }
    return 0;
}

int cdatabase_clear(unsigned int id){
    return 0;
}

int cdatabase_increase(unsigned int id){
    for (size_t i = 0; i < (cdb->n);i ++){
        if(cdb->a[i].id == id){
            (cdb->a[i].count)++;
            return 0;
        }
    }
    return 1;
}

int cdatabase_decrese(unsigned int id){
    for (size_t i = 0; i < (cdb->n);i ++){
        if(cdb->a[i].id == id){
            (cdb->a[i].count)--;
            return 0;
        }
    }
    return 0;
}

int blist_init(){
    b_list = (bldb_t*)malloc(sizeof(bldb_t));
    if (b_list == NULL) return 1;
    b_list->n = 0;
    return 0;
}

int blist_add(unsigned int id){
    b_list->id[(b_list->n)] = id;
    (b_list->n)++;
    return 0;
}

int blist_has(unsigned int id){
    for (size_t i = 0; i < (b_list->n);i ++){
        if(b_list->id[i] == id){
            return 1;
        }
    }
    return 0;
}