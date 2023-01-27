//
// Created by ksmubasshir on 1/27/23.
//
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/stat.h>

#define MAX_THREADS 10
#define BUFFER_SIZE 1024

int server_fd, new_socket;
struct sockaddr_in address;
pthread_t thread_pool[MAX_THREADS];
int thread_count = 0;

void *client_handler(void *socket_desc) {
    char buffer[BUFFER_SIZE] = {0};
    int valread = read(*(int *)socket_desc, buffer, BUFFER_SIZE);
    if (valread < 0) {
        perror("read failed");
        return NULL;
    }

    char method[8], url[BUFFER_SIZE], http_version[16];
    sscanf(buffer, "%s %s %s", method, url, http_version);

    if (strcmp(method, "GET") != 0) {
        send(*(int *)socket_desc, "400 Bad Request\n", 17, 0);
        return NULL;
    }

    if (strcmp(http_version, "HTTP/1.1") != 0) {
        send(*(int *)socket_desc, "505 HTTP Version Not Supported\n", 32, 0);
        return NULL;
    }

    struct stat file_stat;
    if (stat(url, &file_stat) < 0) {
        send(*(int *)socket_desc, "404 Not Found\n", 14, 0);
        return NULL;
    }

    if (!S_ISREG(file_stat.st_mode)) {
        send(*(int *)socket_desc, "404 Not Found\n", 14, 0);
        return NULL;
    }

    char *html_file = malloc(file_stat.st_size + 1);
    FILE *file = fopen(url, "r");
    fread(html_file, file_stat.st_size, 1, file);
    fclose(file);

    send(*(int *)socket_desc, "200 OK\n", 7, 0);
    send(*(int *)socket_desc, html_file, file_stat.st_size, 0);

    free(html_file);

    return NULL;
}

int main(int argc, char const* argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1"),
            address.sin_port = htons(atoi(argv[1]));

    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("The server is ready to receive\n");
    while (1){
        if ((new_socket
                     = accept(server_fd, (struct sockaddr*)&address,
                              (socklen_t*)&addrlen)) < 0){
            perror("accept");
            exit(EXIT_FAILURE);
        }
        // Create a new thread for each client
        pthread_t client_thread;
        int* new_sock = malloc(sizeof(int));
        *new_sock = new_socket;
        if (pthread_create(&client_thread, NULL, client_handler, (void*)new_sock) < 0) {
            perror("could not create thread");
            return 1;
        }
    }
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}