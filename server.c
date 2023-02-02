// Server side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>

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
		char sentence[1024] = { 0 };
		valread = read(new_socket, sentence, 1024);

//        char *client_ip = inet_ntoa(address.sin_addr);
//        int client_port = ntohs(address.sin_port);

        char client_ip[16];
        struct sockaddr_in client_address;
        bzero(&client_address, sizeof(client_address));
        socklen_t len = sizeof(client_address);
        getsockname(server_fd, (struct sockaddr *) &client_address, &len);
        inet_ntop(AF_INET, &address.sin_addr, client_ip, sizeof(client_ip));
        int client_port = ntohs(client_address.sin_port);

		printf("message-from-client: %s, %d\n", client_ip, client_port);
		printf("%s\n", sentence);

		send(new_socket, sentence, strlen(sentence), 0);

		// closing the connected socket
		close(new_socket);
	}
	// closing the listening socket
	shutdown(server_fd, SHUT_RDWR);
	return 0;
}
