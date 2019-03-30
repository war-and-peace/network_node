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
    int socklisten = *((int*)args);
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

        int v;
        memset(buffer, 0, sizeof(buffer));
        nread = recv(sock, (char*)&v, sizeof(v), 0);
        
        fprintf(stderr, "SERVER: value: %d\n", v);

        if (v) {
            memset(buffer, 0, sizeof(buffer));
            nread = recv(sock, buffer, sizeof(buffer), 0);
            string node_info = init_string_c(buffer);
            fprintf(stderr, "SERVER: node info:\t"); sprintln(node_info);
            int f_num;
            fprintf(stderr, "sizeof(fnum): %d\n", sizeof(f_num));
            nread = recv(sock, (char *)&f_num, sizeof(f_num), 0);
            fprintf(stderr, "received bytes: %d\n", nread);
            printf("SERVER: known node: %d\n", f_num);
            fprintf(stderr, "Number of next nodes %d\n", f_num);
            svector_t known_nodes = init_svector();
            int limit;
            if (f_num < 10)
                limit = f_num;
            else
                limit = 10;

            for (int i = 0; i < limit; i++) {
                nread = recv(sock, buffer, sizeof(buffer), 0);
                string node_i = init_string_c(buffer);
                printf("SERVER: next node: ");
                sprintln(node_i);
                known_nodes = svector_add(known_nodes, node_i);
            }

            printf("SERVER: Received all the data\n");
            resolve_sync(node_info, limit, known_nodes);
        } else {
            nread = recv(sock, buffer, sizeof(buffer), 0);
            string file_name = init_string_c(buffer);
            fprintf(stderr, "CLIENT is requesting: \t");
            sprintln(file_name);
            if (!svector_contains(*mfiles, file_name)) {
                int result = 0;
                fprintf(stderr, "SERVER: file: we don't have this file");
                nread = send(sock, &result, sizeof(int), 0);
            } else {
                FILE *f = fopen(buffer, "r");
                if (f == NULL) {
                    fprintf(stderr, "SERVER: FILE: Can't open the file\n");
                    perror("Opening the file");
                    int result = 0;
                    nread = send(sock, (char*)&result, sizeof(result), 0);
                } else {
                    fprintf(stderr, "FILE: SERVER: FILE: opened the file\n");
                    int n = count_words(f);
                    if (n > 0) n--;
                    fprintf(stderr, "FILE: SERVER: Number of words: %d\n", n);
                    int nn = n;
                    nread = send(sock, (char*)&nn, sizeof(nn), 0);
                    fclose(f);
                    FILE *g = fopen(buffer, "r");

                    for (int nCount = 0; nCount < n;nCount ++){
                        char str[1024];
                        fscanf(g, "%s", str);
                        string word = init_string_c(str);
                        word = append_c(word, " ");
                        sprintln(word);
                        char *sending = to_char(word);
                        fprintf(stderr, "FILE: SERVER: current word: %s\n", str);
                        fprintf(stderr, "FILE: SERVER: word length: %lu\n", strlen(sending));
                        nread = send(sock, sending, strlen(sending), 0);
                        fprintf(stderr, "FILE: SERVER: sent bytes: %d\n", nread);
                        // memset(str, 0, sizeof(str));
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
        sleep(3);
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
                fprintf(stderr, "CLIENT: Why it is printing this?\n");
            }
            int sync_p = 1;

            int n = 1;
            printf("done here %d\n", connect_n);
            nbytes = send(sock, (char*)&n, sizeof(n), MSG_DONTWAIT);
            string my_node = my_node_init();
            char *mnbuf = to_char(my_node);
            sprintln(my_node);
            fprintf(stderr, "my node info: %s\n", mnbuf);
            printf("length: %lu\n", strlen(mnbuf));
            nbytes = send(sock, mnbuf, strlen(mnbuf) + 1, MSG_DONTWAIT);
            printf("sent bytes: %d\n", nbytes);
            size_t n_known = db->n;
            n = db->n;
            printf("db n: %d\n", n);
            nbytes = send(sock, (char*)&n, sizeof(n), MSG_DONTWAIT);
            fprintf(stderr, "Sent bytes: %d\n", nbytes);
            for (size_t k = 0; k < (db->n); k++) {
                node_t next_n = db->known_nodes[k];
                string message = get_message(next_n);
                printf("message: \t");
                sprintln(message);
                char *mbuf = to_char(message);
                nbytes = send(sock, mbuf, strlen(mbuf) + 1, MSG_DONTWAIT);
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
            fprintf(stderr, "Did not receive correct file name\n");
            continue;
        }

        string file_n = init_string_c(file_name);
        fprintf(stderr, "Requesting the file with name: ");
        sprintln(file_n);
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
        if(sock < 0){
            fprintf(stderr, "CLIENT: file: ");
            perror("socket creation");
            continue;
        }

        int result = connect(sock, (struct sockaddr *)&dest, sizeof(dest));

        if(result != 0){
            perror("CLIENT: file: connecting ");
            close(sock);
            continue;
        }

        printf("CLIENT: file: Connection established!\n");

        int req_p = 0;
        nbytes = send(sock, &req_p, sizeof(req_p), 0);

        char *fbuf = to_char(file_n);
        nbytes = send(sock, fbuf, strlen(fbuf) + 1, 0);
        fprintf(stderr, "CLIENT: file: sent file name: %s\n", fbuf);

        int nwords;
        nbytes = recv(sock, &nwords, sizeof(nwords), 0);
        fprintf(stderr, "CLIENT: FILE: received bytes: %d\n", nbytes);
        fprintf(stderr, "CLIENT: file: received byte count: %d\n", nwords);

        FILE *f = fopen(to_char(file_n), "w");
        if(f == NULL){
            perror("CLIENT: file: file open");
            continue;
        }
        
        for (int i = 0; i < nwords; i++) {
            char buffer[1024];
            nbytes = recv(sock, buffer, sizeof(buffer), 0);
            buffer[nbytes] = '\0';
            fprintf(stderr, "CLIENT: FILE: Received number of bytes: %d\n", nbytes);
            fprintf(stderr, "CLIENT: FILE: Received word: %s", buffer);
            fprintf(f, "%s", buffer);
        }
        fclose(f);
        close(sock);
    }
}

void resolve_sync(string node_info, int n, svector_t nodes_i){
    fprintf(stderr, "SERVER: Resolving sync\n");
    fprintf(stderr, "node_info: ");
    sprintln(node_info);
    fprintf(stderr, "known_nodes number: %d\n", n);
    fprintf(stderr, "SERVER: nodes size: %lu\n", *(nodes_i._size));
    for (int i = 0; i < *(nodes_i._size);i ++){
        fprintf(stderr, "SERVER: nodes: ");
        sprintln(nodes_i.data[i]);
    }
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
    while(!feof(f)){
        fscanf(f, "%s", s);
        cnt++;
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
    sprintln(*(node.name));
    sprintln(*(node.ip));
    sprintln(*(node.port));
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