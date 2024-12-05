#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>

#define BUFFER_SIZE 256


void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        exit(1);
    }

    int socket_fd, port_number;
    struct sockaddr_in server_address;
    int server_len = sizeof(server_address);

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_fd < 0) {
        error("Error opening socket");
    }

    struct hostent * server = gethostbyname(argv[1]);

    if (server == NULL) {
        error("No server found");
    }

    bzero((char *) &server_address, server_len);
    port_number = atoi(argv[2]);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_number);
    bcopy((char *)server->h_addr_list[0], (char *) &server_address.sin_addr.s_addr, server->h_length);

    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);

    printf("Write a message: ");
    scanf(" %255[^\n]", buffer);

    int message_size = strlen(buffer) + 1;
    if (sendto(socket_fd, buffer, message_size, 0, (struct sockaddr *) &server_address, server_len) < 0) {
        error("Error sending message");
    }

    bzero(buffer, BUFFER_SIZE);

    if (recvfrom(socket_fd, buffer, message_size, 0, NULL, NULL) < 0) {
        error("Error reading from socket");
    }

    printf("%s\n", buffer);

    return 0;
}