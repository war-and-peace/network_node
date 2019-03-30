#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "dbase.h"

#define SERVER_PORT 2000
#define SERVER_PORT_C "2000"
#define h_addr h_addr_list[0]
#define BUFFER_SIZE 1024
#define IP "127.0.0.1"
#define PORT "2003"
#define NAME "RA"
#define MY_IP "192.168.1.16"

typedef struct _flag {
    int v;
} flag_t;

typedef struct _client_info{
    int type;
    char ip[30];
} client_info_t;

flag_t* flag;

void* server_thread(void* args);
void* client_ping_thread(void* args);
void* client_file_thread(void* args);
void resolve_sync(string node_info, int n, svector_t nodes_i);