#include "connection.h"

svector_t parse_file_names(string n_files);
int count_words(FILE* f);
string my_node_init();
string get_message(node_t node);
string get_file_name();
node_t get_file_node(string file_n);

void setup_tcp_server_communication(){
    int master_sock_tcp_fd = 0, sent_recv_bytes = 0, addr_len = 0, opt = 1;
    int comm_socket_fd = 0;     
    fd_set readfds;               
    struct sockaddr_in server_addr, client_addr;
    if ((master_sock_tcp_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP )) == -1){
        printf("socket creation failed\n");
        exit(1);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = SERVER_PORT;
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    addr_len = sizeof(struct sockaddr);

    if (bind(master_sock_tcp_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1){
        printf("socket bind failed\n");
        return;
    }
    
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(master_sock_tcp_fd, (struct sockaddr *)&sin, &len) == -1)
        perror("getsockname");
    else
        printf("port number %d\n", ntohs(sin.sin_port));

    if (listen(master_sock_tcp_fd, 5)<0)  
    {
        printf("listen failed\n");
        return;
    }
    while(1){

        FD_ZERO(&readfds);                     
        FD_SET(master_sock_tcp_fd, &readfds);  

        printf("blocked on select System call...\n");

        select(master_sock_tcp_fd + 1, &readfds, NULL, NULL, NULL); 
       
        if (FD_ISSET(master_sock_tcp_fd, &readfds)){    
            printf("SERVER: New connection recieved recvd, accept the connection. Client and Server completes TCP-3 way handshake at this point\n");
            comm_socket_fd = accept(master_sock_tcp_fd, (struct sockaddr *)&client_addr, &addr_len);
            
            if(comm_socket_fd < 0){
                printf("SERVER: accept error : errno = %d\n", errno);
                exit(0);
            }

            printf("SERVER: Connection accepted from client : %s:%u\n", 
                inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            while(1){
                memset(data_buffer, 0, sizeof(data_buffer));
                sent_recv_bytes = recvfrom(comm_socket_fd, (char *)data_buffer, sizeof(data_buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
                printf("SERVER: Server recvd %d bytes from client %s:%u\n", sent_recv_bytes, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                flag = (flag_t*)data_buffer;
                if(flag->v){
                    sent_recv_bytes = recvfrom(comm_socket_fd, (char *)data_buffer, sizeof(data_buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
                    string node_info = init_string_c(data_buffer);
                    sent_recv_bytes = recvfrom(comm_socket_fd, (char *)data_buffer, sizeof(data_buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
                    int* n_files = (int*)data_buffer;
                    svector_t known_nodes = init_svector();
                    for(int i = 0;i < *n_files;i ++){
                        sent_recv_bytes = recvfrom(comm_socket_fd, (char *)data_buffer, sizeof(data_buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
                        string node_i = init_string_c(data_buffer);
                        known_nodes = svector_add(known_nodes, node_i);
                    }
                    resolve_sync(node_info, *n_files, known_nodes);
                    break;
                }else{
                    sent_recv_bytes = recvfrom(comm_socket_fd, (char *)data_buffer, sizeof(data_buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
                    string file_name = init_string_c(data_buffer);
                    
                    if(!svector_contains(*mfiles, file_name)){
                        int result = 0;
                        sent_recv_bytes = sendto(comm_socket_fd, (char *)&result, sizeof(int), 0, (struct sockaddr *)&client_addr, sizeof(struct sockaddr));
                    }else{
                        FILE* f = fopen(data_buffer, "r");
                        if(f == NULL){
                            perror("Opening the file");
                            int result = 0;
                            sent_recv_bytes = sendto(comm_socket_fd, (char *)&result, sizeof(int), 0, (struct sockaddr *)&client_addr, sizeof(struct sockaddr));
                        }else{
                            int n = count_words(f);
                            sent_recv_bytes = sendto(comm_socket_fd, (char *)&n, sizeof(int), 0, (struct sockaddr *)&client_addr, sizeof(struct sockaddr));
                            fclose(f);
                            FILE* g = fopen(data_buffer, "r");
                            char str[1024];
                            while(fscanf(g, "%s", str)){
                                sent_recv_bytes = sendto(comm_socket_fd, (char *)&str, sizeof(char[1024]), 0, (struct sockaddr *)&client_addr, sizeof(struct sockaddr));
                            }
                            fclose(g);
                        }
                    }
                }
                break;
            }
       }
   } 
}

// ----------------------------------Client connection implementation----------------------------------
// ----------------------------------Client connection implementation----------------------------------
// ----------------------------------Client connection implementation----------------------------------
// ----------------------------------Client connection implementation----------------------------------


void* setup_client_tcp_communication(void* args) {
    
    int sockfd = 0, sent_recv_bytes = 0;
    int addr_len = 0;
    addr_len = sizeof(struct sockaddr);
    struct sockaddr_in dest;
    int type = 1;
    int cur = 0;
    while(1) {
        sleep(1);
        cur ++;
        type = 1;
        if(cur % 5 == 0){
            type = 0;
        }

        if (type)
        {
            for (size_t i = 0; i < db->n; i++)
            {
                printf("db size: %d\n", db->n);
                node_t node = db->known_nodes[i];
                sprintln(*(node.ip));
                sprintln(*(node.port));
                char *server_ip = to_char(*(node.ip));
                dest.sin_family = AF_INET;
                dest.sin_port = to_int(*(node.port));
                struct hostent *host = (struct hostent *)gethostbyname(server_ip);
                dest.sin_addr = *((struct in_addr *)host->h_addr);

                sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

                connect(sockfd, (struct sockaddr *)&dest, sizeof(struct sockaddr));

                printf("CLIENT: Connection established!\n");

                int sync_p = 1;
                sent_recv_bytes = sendto(sockfd, &sync_p, sizeof(int), 0, (struct sockaddr *)&dest, sizeof(struct sockaddr));
                string my_node = my_node_init();
                char *mnbuf = to_char(my_node);
                sent_recv_bytes = sendto(sockfd, mnbuf, sizeof(char[1024]), 0, (struct sockaddr *)&dest, sizeof(struct sockaddr));
                size_t n_known = db->n;
                sent_recv_bytes = sendto(sockfd, &n_known, sizeof(size_t), 0, (struct sockaddr *)&dest, sizeof(struct sockaddr));
                for (size_t k = 0; k < db->n; k++)
                {
                    node_t next_n = db->known_nodes[k];
                    string message = get_message(next_n);
                    char *mbuf = to_char(message);
                    sent_recv_bytes = sendto(sockfd, &mbuf, sizeof(char[1024]), 0, (struct sockaddr *)&dest, sizeof(struct sockaddr));
                }
            }
        } else {

            string file_n = get_file_name();
            if (size(file_n) == 0){
                continue;
            }

            node_t node = get_file_node(file_n);
            char *server_ip = to_char(*(node.ip));
            dest.sin_family = AF_INET;
            dest.sin_port = to_int(*(node.port));
            struct hostent *host = (struct hostent *)gethostbyname(server_ip);
            dest.sin_addr = *((struct in_addr *)host->h_addr);

            sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

            connect(sockfd, (struct sockaddr *)&dest, sizeof(struct sockaddr));

            printf("CLIENT: Connection established!\n");

            int req_p = 0;
            sent_recv_bytes = sendto(sockfd, &req_p, sizeof(int), 0, (struct sockaddr *)&dest, sizeof(struct sockaddr));

            char *fbuf = to_char(file_n);
            sent_recv_bytes = sendto(sockfd, &fbuf, sizeof(char[1024]), 0, (struct sockaddr *)&dest, sizeof(struct sockaddr));

            int nwords;
            sent_recv_bytes =  recvfrom(sockfd, (char *)&nwords, sizeof(int), 0, (struct sockaddr *)&dest, &addr_len);

            FILE* f = fopen(to_char(file_n), "w");
            char buffer[1024];
            for(int i = 0;i < nwords;i ++){
                sent_recv_bytes =  recvfrom(sockfd, (char *)&buffer, 1024, 0, (struct sockaddr *)&dest, &addr_len);
                fprintf(f, "%s", buffer);
            }
            fclose(f);
        }
    }
}


void resolve_sync(string node_info, int n, svector_t nodes_i){
    char name[100];
    char ip[30];
    char port[10];
    char files[1024];
    int t = 0, current = 0;
    for(size_t i = 0;i < size(node_info);i ++){
        if(t == 0){
            if(node_info.data[i] == ':'){
                name[current] = '\0';
                current = 0;
                t ++;
            }else{
                name[current] = node_info.data[i];
                current ++;
            }
            continue;
        }
        if(t == 1){
            if(node_info.data[i] == ':'){
                ip[current] = '\0';
                current = 0;
                t ++;
            }else{
                ip[current] = node_info.data[i];
                current ++;
            }
            continue;
        }
        if(t == 2){
            if(node_info.data[i] == ':'){
                port[current] = '\0';
                current = 0;
                t ++;
            }else{
                port[current] = node_info.data[i];
                current ++;
            }
            continue;
        }
        if(t == 3){
            if(node_info.data[i] == ':'){
                files[current] = '\0';
                current = 0;
                t ++;
            }else{
                files[current] = node_info.data[i];
                current ++;
            }
            continue;
        }
    }
    files[current] = '\0';
    string n_name = init_string_c(name);
    string n_ip = init_string_c(ip);
    string n_port = init_string_c(port);
    // sprintln(n_name);
    // sprintln(n_ip);
    // sprintln(n_port);
    string n_files = init_string_c(files);
    n_files = erase(n_files, 0, 1);
    n_files = erase(n_files, size(n_files) - 1, 1);
    // sprintln(n_files);
    svector_t files_list = parse_file_names(n_files);
    node_t node = init_node();
    *(node.name) = init_string_s(n_name);
    *(node.ip) = init_string_s(n_ip);
    *(node.port) = init_string_s(n_port);
    database_add(node);
    // perror("done");
    database_update(node, files_list);
    // perror("DONE2");
    for(size_t k = 0;k < *(nodes_i._size);k ++){
        string p = init_string_s(nodes_i.data[k]);
        // printf("p = ");sprintln(p);
        current = 0;
        char name[1024];
        char ip[30];
        char port[10];
        t = 0;

        for (size_t i = 0; i < size(p); i++){
            if (t == 0){
                if (p.data[i] == ':'){
                    name[current] = '\0';
                    current = 0;
                    t++;
                } else {
                    name[current] = p.data[i];
                    current++;
                }
                continue;
            }
            if (t == 1){
                if (p.data[i] == ':'){
                    ip[current] = '\0';
                    current = 0;
                    t++;
                }else{
                    ip[current] = p.data[i];
                    current++;
                }
                continue;
            }
            if (t == 2){
                if (p.data[i] == ':'){
                    port[current] = '\0';
                    current = 0;
                    t++;
                }else{
                    port[current] = p.data[i];
                    current++;
                }
                continue;
            }
        }
        port[current] = '\0';

        node_t node = init_node();
        *(node.name) = init_string_c(name);
        *(node.ip) = init_string_c(ip);
        *(node.port) = init_string_c(port);
        database_add(node);
    }
}

svector_t parse_file_names(string n_files){
    char b[1024];
    int current = 0;
    svector_t v = init_svector();
    for(size_t i = 0;i < size(n_files);i ++){
        if(n_files.data[i] == ','){
            b[current] = '\0';
            v = svector_add(v, init_string_c(b));
            current = 0;
        }else{
            b[current] = n_files.data[i];
            current ++;
        }
    }
    b[current] = '\0';
    v = svector_add(v, init_string_c(b));
    return v;
}

int count_words(FILE* f){
    char s[1024];
    int cnt = 0;
    while(fscanf(f, "%s", s)){
        cnt ++;
    }
    return cnt;
}

string my_node_init(){
    string my_node = init_string();
    my_node = append_c(my_node, NAME);
    my_node = append_c(my_node, ":");
    my_node = append_c(my_node, MY_IP);
    my_node = append_c(my_node, ":");
    my_node = append_c(my_node, SERVER_PORT);
    my_node = append_c(my_node, ":");
    my_node = append_c(my_node, "[");
    size_t n_files = *(mfiles->_size);
    for (size_t k = 0; k < n_files; k++)
    {
        if (k == n_files - 1)
        {
            my_node = append(my_node, mfiles->data[k]);
        }
        else
        {
            my_node = append(my_node, mfiles->data[k]);
            my_node = append_c(my_node, ",");
        }
    }
    my_node = append_c(my_node, "]");
    return my_node;
}

string get_message(node_t node){
    string message = init_string();
    message = append(message, *(node.name));
    message = append_c(message, ":");
    message = append(message, *(node.ip));
    message = append_c(message, ":");
    message = append(message, *(node.port));
    return message;
}

string get_file_name(){
    size_t n = db->n;
    for(size_t i = 0;i < n;i ++){
        svector_t v = *(db->known_nodes[i].files);
        size_t nn = *(v._size);
        for(size_t k = 0;k < nn;k ++){
            int has = 0;
            for(size_t q = 0;q < *(mfiles->_size);q ++){
                if(equal(v.data[k], mfiles->data[q])){
                    has = 1;
                    break;
                }
            }
            if(!has){
                return v.data[k];
            }
        }
    }
    return init_string();
}

node_t get_file_node(string file_n){
    size_t n = db->n;
    for(size_t i = 0;i < n;i ++){
        svector_t v = *(db->known_nodes[i].files);
        size_t nn = *(v._size);
        for(size_t k = 0;k < nn;k ++){
            if(equal(v.data[k], file_n)){
                return db->known_nodes[i];
            }
        }
    }
}