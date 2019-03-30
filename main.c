// #include "dbase.h"
#include "connection.h"
#include <pthread.h>

#define POOL_SIZE 10

dbase_t* db;
svector_t* mfiles;
pthread_t* threads;
pthread_t cthread_ping, cthread_file;

void init_files() {
    mfiles = (svector_t*)malloc(sizeof(svector_t));
    (*mfiles) = init_svector();
    (*mfiles) = svector_add((*mfiles), init_string_c("ra.txt"));
}

int main(){
    init_files();
    
    threads = malloc(sizeof(pthread_t) * POOL_SIZE);
    if(threads == NULL){
        fprintf(stderr, "Can't allocate memory for threads");
        exit(1);
    }

    struct sockaddr_in sAddr;
    int listensock;
    int result;
    int val;

    listensock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(listensock < 0){
        perror("socket creation");
        exit(1);
    }

    val = 1;
    result = setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (result < 0) {
        perror("SERVER");
        return 0;
    }

    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(SERVER_PORT);
    sAddr.sin_addr.s_addr = INADDR_ANY;
    result = bind(listensock, (struct sockaddr*)&sAddr, sizeof(sAddr));
    if (result < 0) {
        perror("SERVER bind");
        return 0;
    }

    result = listen(listensock, 5);
    if (result < 0) {
        perror("SERVER");
        return 0;
    }

    char i_buffer[30];
    fscanf(stdin, "%s", i_buffer);
    if (strcmp(i_buffer, "0") == 0) {
        if (initialize_database_clean()) return 1;
    } else {
        string node_i = init_string_c(i_buffer);
        int counter = 0;
        char b1[30], b2[10];
        while (counter < size(node_i)) {
            if (node_i.data[counter] == ':') break;
            b1[counter] = node_i.data[counter];
            counter++;
        }
        b1[counter] = '\0';
        counter++;
        int counter2 = 0;
        while (counter < size(node_i)) {
            b2[counter2] = node_i.data[counter];
            counter2++;
            counter++;
        }
        b2[counter2] = '\0';
        if (initialize_database(init_string_c(b1), init_string_c(b2), init_string_c("VM"))) return 1;
    }

    for (int i = 0; i < POOL_SIZE; i++) {
        fprintf(stderr, "point: %d\n", i);
        result = pthread_create(&threads[i], NULL, server_thread,
                                (void*)listensock);
        if (result != 0) {
            printf("SERVER: Could not create thread %d\n", i);
        }
        sched_yield();
    }
    pthread_create(&cthread_ping, NULL, client_ping_thread, 0);
    pthread_create(&cthread_file, NULL, client_file_thread, 0);
    for (int i = 0; i < POOL_SIZE;i ++){
        pthread_join(threads[i], NULL);
    }

    pthread_join(cthread_ping, NULL);
    pthread_join(cthread_file, NULL);
    return 0;
}