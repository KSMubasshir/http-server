//
// Created by ksmubasshir on 2/3/23.
//
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <regex.h>

#define MAX_BUF_SIZE 1024

int main(int argc, char const *argv[]) {
    int sock = 0, valread, client_fd;
    struct sockaddr_in serv_addr;
    char buffer[MAX_BUF_SIZE] = {0};


    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    char url[100];
    strcpy(url, argv[1]);

    char host[100], path[100];
    int port = 80;

    regex_t re;
    regcomp(&re, "http://([a-zA-Z0-9\\.]+):([0-9]+)(/[a-zA-Z0-9\\./]+)", REG_EXTENDED);
    regmatch_t match[4];
    if (regexec(&re, url, 4, match, 0) == 0) {
        int start = match[1].rm_so;
        int end = match[1].rm_eo;
        int len = end - start;
        strncpy(host, url + start, len);
        host[len] = '\0';

        start = match[2].rm_so;
        end = match[2].rm_eo;
        len = end - start;
        strncpy(buffer, url + start, len);
        buffer[len] = '\0';
        port = atoi(buffer);

        start = match[3].rm_so;
        end = match[3].rm_eo;
        len = end - start;
        strncpy(path, url + start, len);
        path[len] = '\0';
    } else {
        printf("Invalid URL format\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, host, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((client_fd = connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Send GET request for base HTML file
    char request[1024];
    sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n\r\n", path, host);
    send(sock, request, strlen(request), 0);

    // Read the response and check if there are more web objects to
    valread = 0;
    char buffer1[MAX_BUF_SIZE] = {0};
    valread = read(sock, buffer1, 1024);
    buffer1[valread] = '\0';

    char *header_end = strstr(buffer1, "\r\n\r\n");
    if (header_end == NULL) {
        header_end = strstr(buffer1, "\n\n");
    }
    if (header_end != NULL) {
        int header_len = header_end - buffer1 + 2;
        char header[header_len + 1];
        memcpy(header, buffer1, header_len);
        header[header_len] = '\0';

        int html_len = valread - header_len;
        char html[html_len + 1];
        memcpy(html, header_end + 4, html_len);
        html[html_len] = '\0';

        printf("%s\n", header);
    } else {
        printf("Failed to extract header and HTML.\n");
    }


    sleep(60);
    // closing the connected socket
    close(client_fd);
    return 0;
}
