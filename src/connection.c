#include "connection.h"

svector_t parse_file_names(string n_files);
int count_words(FILE* f);
string my_node_init();
string get_message(node_t node);
string get_file_name();
node_t get_file_node(string file_n);

typedef struct _str{
	char data[1024];
} sstr;

void *server_thread(void *args) {
    fprintf(stderr, "SERVER thread working\n");
    int socklisten = (int)args;
    char buffer[BUFFER_SIZE];
    int nread;
    struct sockaddr_in client_addr;
    int addr_len = sizeof(client_addr);

    while (1) {
        int sock = accept(socklisten, (struct sockaddr *)&client_addr, &addr_len);

        if (sock < 0) {
            perror("SERVER:[THREAD] accept");
            continue;
        }

        fprintf(stderr, "SERVER: Connection accepted from client : %s:%u\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        flag_t num;
        memset(buffer, 0, sizeof(buffer));
        nread = recv(sock, (char*)&num, sizeof(flag_t), 0);
        
        fprintf(stderr, "SERVER: value: %d\n", num.v);

        if (num.v) {
            memset(buffer, 0, sizeof(buffer));
            nread = recv(sock, buffer, sizeof(buffer), 0);
            string node_info = init_string_c(buffer);
            fprintf(stderr, "SERVER: node info:\t"); sprintln(node_info);
            flag_t files_n;
            nread = recv(sock, &files_n, sizeof(flag_t), 0);
            printf("SERVER: known node: %d\n", files_n.v);
            svector_t known_nodes = init_svector();
            for (int i = 0; i < files_n.v; i++) {
                nread = recv(sock, buffer, sizeof(buffer), 0);
                string node_i = init_string_c(buffer);
                printf("SERVER: next node: ");
                sprintln(node_i);
                known_nodes = svector_add(known_nodes, node_i);
            }

            printf("SERVER: Received all the data\n");
            resolve_sync(node_info, files_n.v, known_nodes);
            break;
        } else {
            nread = recv(sock, buffer, sizeof(buffer), 0);
            string file_name = init_string_c(buffer);
            fprintf(stderr, "CLIENT is requesting: \t");
            sprintln(file_name);
            if (!svector_contains(*mfiles, file_name)) {
                int result = 0;
                nread = send(sock, &result, sizeof(int), 0);
            } else {
                // fprintf(stderr, "Opening the file to send\n");
                FILE *f = fopen(buffer, "r");
                if (f == NULL) {
                    perror("Opening the file");
                    flag_t result;
                    result.v = 0;
                    nread = send(sock, &result, sizeof(flag_t), 0);
                } else {
                    int n = count_words(f);
                    flag_t nn;
                    nn.v = n;
                    nread = send(sock, &n, sizeof(flag_t), 0);
                    fclose(f);
                    FILE *g = fopen(buffer, "r");
                    char str[1024];
                    while (fscanf(g, "%s", str)) {
                        nread = send(sock, str, strlen(str) + 1, 0);
                    }
                    fclose(g);
                }
            }
        }
        close(sock);
    }
}

// ----------------------------------Client connection implementation----------------------------------
// ----------------------------------Client connection implementation----------------------------------
// ----------------------------------Client connection implementation----------------------------------
// ----------------------------------Client connection implementation----------------------------------

void* client_ping_thread(void* args) {
    fprintf(stderr, "Client ping thread\n");
    while (1) {
        sleep(1);
        size_t size = db->n;
        for (size_t i = 0; i < size; i++) {
            node_t node = db->known_nodes[i];
            fprintf(stderr, "CLIENT: connecting to: ");
            sprint(*(node.ip));
            sprintln(*(node.port));

            struct sockaddr_in dest;
            char *server_ip = to_char(*(node.ip));
            dest.sin_family = AF_INET;
            dest.sin_port = htons(to_int(*(node.port)));
            struct hostent *host = (struct hostent *)gethostbyname(server_ip);
            dest.sin_addr = *((struct in_addr *)host->h_addr);
            int nbytes;

            int sock = socket(AF_INET, SOCK_STREAM, 0);

            if (sock == -1) {
                perror("CLIENT socket creation");
                continue;
            }
            
            int connect_n = connect(sock, (struct sockaddr *)&dest, sizeof(dest));
            if (connect_n != 0) {
                fprintf(stderr, "ERROR in connecting: %d\n", errno);
                close(sock);
                continue;
            }
            if (connect_n) continue;
            if (connect_n != 0) {
                fprintf(stderr, "CLIENT: Connection established!\n");
                fprintf(stderr, "CLIENT: WHy it is printing this?\n");
            }
            int sync_p = 1;

            flag_t num;
            num.v = 1;
            printf("done here %d\n", connect_n);
            nbytes = send(sock, &num, sizeof(num), 0);
            // sent_recv_bytes = sendto(sockfd, (char *)&num, sizeof(flag_t), 0, (struct sockaddr *)&dest, sizeof(struct sockaddr));
            // printf("number of bytes sent: %d\n", sent_recv_bytes);
            string my_node = my_node_init();
            char *mnbuf = to_char(my_node);
            sprintln(my_node);
            fprintf(stderr, "my node info: %s\n", mnbuf);
            printf("length: %d\n", strlen(mnbuf));
            nbytes = send(sock, &mnbuf, strlen(mnbuf) + 1, 0);
            // sent_recv_bytes = sendto(sockfd, (char *)mnbuf, strlen(mnbuf) + 1, 0, (struct sockaddr *)&dest, sizeof(struct sockaddr));
            printf("sent bytes: %d\n", nbytes);
            size_t n_known = db->n;
            num.v = db->n;
            printf("db n: %d\n", num.v);
            nbytes = send(sock, &num, sizeof(num), 0);
            // sent_recv_bytes = sendto(sockfd, (char *)&num, sizeof(flag_t), 0, (struct sockaddr *)&dest, sizeof(struct sockaddr));
            for (size_t k = 0; k < (db->n); k++) {
                node_t next_n = db->known_nodes[k];
                string message = get_message(next_n);
                printf("message: \t");
                sprintln(message);
                char *mbuf = to_char(message);
                nbytes = send(sock, mbuf, strlen(mbuf) + 1, 0);
                // sent_recv_bytes = sendto(sockfd, (char *)mbuf, strlen(mbuf) + 1, 0, (struct sockaddr *)&dest, sizeof(struct sockaddr));
            }
            close(sock);
            
        }
    }
}

void* client_file_thread(void* args) {
    fprintf(stderr, "Client file thread\n");
    char file_name[30];
    while (1) {
        if(fscanf(stdin, "%s", file_name) != 1){
            printf(stderr, "Did not receive correct file name\n");
            continue;
        }

        string file_n = init_string_c(file_name);
        
        node_t node = get_file_node(file_n);
        if(node.ip == NULL){
            fprintf(stderr, "Don't know where to find this file. Check file name\n");
            continue;
        }

        int sock;
        int nbytes;
        struct sockaddr_in dest;
        char *server_ip = to_char(*(node.ip));
        dest.sin_family = AF_INET;
        dest.sin_port = htons(to_int(*(node.port)));
        struct hostent *host = (struct hostent *)gethostbyname(server_ip);
        dest.sin_addr = *((struct in_addr *)host->h_addr);

        sock = socket(AF_INET, SOCK_STREAM, 0);

        connect(sock, (struct sockaddr *)&dest, sizeof(dest));

        printf("CLIENT: Connection established!\n");

        flag_t req_p;
        req_p.v = 0;
        nbytes = send(sock, &req_p, sizeof(req_p), 0);
        // sent_recv_bytes = sendto(sockfd, (char *)&req_p, sizeof(flag_t), 0, (struct sockaddr *)&dest, sizeof(struct sockaddr));

        char *fbuf = to_char(file_n);
        nbytes = send(sock, fbuf, strlen(fbuf) + 1, 0);
        // sent_recv_bytes = sendto(sockfd, (char *)&fbuf, strlen(fbuf) + 1, 0, (struct sockaddr *)&dest, sizeof(struct sockaddr));

        flag_t nwords;
        nbytes = recv(sock, &nwords, sizeof(nwords), 0);
        // sent_recv_bytes = recvfrom(sockfd, (char *)&nwords, sizeof(flag_t), 0, (struct sockaddr *)&dest, &addr_len);

        FILE *f = fopen(to_char(file_n), "w");
        char buffer[1024];
        for (int i = 0; i < nwords.v; i++) {
            nbytes = recv(sock, buffer, sizeof(buffer), 0);
            // sent_recv_bytes = recvfrom(sockfd, (char *)&buffer, 1024, 0, (struct sockaddr *)&dest, &addr_len);
            fprintf(f, "%s", buffer);
        }
        fclose(f);
        close(sock);
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
        if(strcmp(ip, MY_IP) == 0){
            continue;
        }
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
    my_node = append_c(my_node, SERVER_PORT_C);
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
    node_t temp = {0};
    return temp;
}