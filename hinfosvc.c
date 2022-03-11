#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h> 
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 4096

void cpuName(char *buffer);
void hostname(char *buffer);
void cpuLoad(char *buffer);
int get_len(char *buffer);

int main(int argc, char* argv[]) {
    socklen_t client_len;
    if (argc != 2) {
        perror("Invalid number of arguments\n");
        exit(EXIT_FAILURE);
    }

    //check if port values are valid
    unsigned long int port = strtoul(argv[1], NULL, 10);
    if ((port > 65535) || (port < 1)) {
        perror("Invalid port number given\n");
        exit(EXIT_FAILURE);
    }

    int sock, newSock;
    //creates a new socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("ERROR in socket");
        exit(EXIT_FAILURE);
    }
    
    //sets values of socket
    int optval = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) != 0) {
        perror("ERROR in socket set up");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address, client_address;

    //empties and then sets values of server_address
    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    //binds socket
    if (bind(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("ERROR: bind");
        exit(EXIT_FAILURE);
    }

    //listens for requests
    if ((listen(sock, 3)) < 0) {
        perror("ERROR: listen");
        exit(EXIT_FAILURE);
    }

    client_len = sizeof(client_address);

    char buffer[BUF_SIZE] = {0, };

    while (true) {
        newSock = accept(sock, (struct sockaddr *) &client_address, &client_len);

        if (newSock < 0){
            perror("ERROR: accept");
            exit(EXIT_FAILURE);
        } else {
            //printf("server: got connection from %s port %d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        }

        if (read(newSock, buffer, BUF_SIZE) < 0) {
            perror("ERROR: read");
            exit(EXIT_FAILURE);
        }
        //HOSTNAME request
        if (strncmp(buffer, "GET /hostname", 13) == 0) {
            hostname(buffer);
        }
        //CPU NAME request
        else if (strncmp(buffer, "GET /cpu-name ", 14) == 0) {
            cpuName(buffer);
        }
        //CPU LOAD request
        else if (strncmp(buffer, "GET /load ", 10) == 0) {
            cpuLoad(buffer);
        }
        //BAD request
        else {
            char *response = "HTTP/1.1 400 Bad Request\r\n\r\n";
            send(newSock, response, get_len(response), 0);
            close(newSock);
            continue;
        }

        //forming result
        char response[BUF_SIZE] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: ";

        char str_len[255];
        sprintf(str_len, "%i", get_len(buffer));
        strcat(response, str_len);
        strcat(response, "\r\n\r\n");
        strcat(response, buffer);
        send(newSock, response, get_len(response), 0);

        close(newSock);
    }
    
    close(sock);
}

//calculating CPU load from /proc/stat
void get_cpu_load(long *stats){
    FILE* fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        printf("Cannot open file\n"); 
        return;
    }

    char cpu_stats[BUF_SIZE] = {0, };
    long field[BUF_SIZE] = {0, };
    char tmp[BUF_SIZE] = {0, };
    int tmp_len = 0;
    int field_len = 0;

    for(int i = 0; i < 5; i++){
        (void)fgetc(fp);
    }

    fgets(cpu_stats, BUF_SIZE, fp);
    fclose(fp);
    
    for(int i = 0; cpu_stats[i] != 0; i++){
        if (cpu_stats[i] == ' '){
            tmp_len = 0;
            field[field_len++] = atol(tmp);
            bzero(tmp, BUF_SIZE);
        } 
        else {
            tmp[tmp_len++] = cpu_stats[i];
        }
    }

    long idle_time = field[3] + field[4]; // idle + iowait

    long total_time = 0;
    for(int i = 0; i < field_len; i++)
        total_time+=field[i];

    stats[0] = idle_time;
    stats[1] = total_time;

}

//calculates cpu load and stores it in buffer variable
void cpuLoad(char *buffer) {
    long prev[2];
    long act[2];
    
    get_cpu_load(prev);
    sleep(1);
    get_cpu_load(act);

    // actual_total - prev_total
    long totald = act[1] - prev[1];
    // actual_idle - prev_idle
    long idled = act[0] - prev[0];

    // (100 * (total_time - idle_time)) / total_time
    float percentage = (100.0 * (totald - idled)) / totald;

    sprintf(buffer, "%f", percentage);
    strcat(buffer, "%\n");
}

//calculates lenght of buffer
int get_len(char *buffer){
    int len = 0;
    for(; buffer[len] != 0; len++);
    return len;
}

//gets cpu name from /proc and stores it in buffer
void cpuName(char *buffer) {
    FILE *fp = popen("cat /proc/cpuinfo | grep 'model name' | head -n1 | awk -F':' '{ print $2 }' | sed -e 's/^ *//g' -e 's/ *$//g'", "r");
    fgets(buffer, BUF_SIZE, fp);
}

//gets hostname from /proc and stores it in buffer
void hostname(char *buffer) {
    FILE *fp = popen("hostname", "r");
    fgets(buffer, BUF_SIZE, fp);
}