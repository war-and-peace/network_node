// #include "dbase.h"
#include "connection.h"
#include <pthread.h>

dbase_t* db;
svector_t* mfiles;


void init_files(){
    mfiles = (svector_t*)malloc(sizeof(svector_t));
    (*mfiles) = init_svector();
    (*mfiles) = svector_add((*mfiles), init_string_c("ra.txt"));
}

int main(){
    init_files();
    if(initialize_database(init_string_c("192.168.1.6"), init_string_c("2000"), init_string_c("VM")))return 1;
    //if(initialize_database_clean())return 1;
    pthread_t thread;
    pthread_create(&thread, NULL, setup_client_tcp_communication, NULL);
    setup_tcp_server_communication();
    pthread_join(thread, NULL);
    return 0;
}