#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>

#define BUFFER_SIZE 256

void error(char *msg) {
    perror(msg);
    exit(1);
}

void string_to_uppercase(char *dest, char *src) {

    int i;
    for (i = 0; src[i] != '\0'; i++) {
        dest[i] = toupper(src[i]);
    }
    dest[i] = '\0';
}

int main(int argc, char *argv[]) {

    if (argc < 1) {
        error("No port provided");
    }

    int socket_fd, port_number;
    struct sockaddr_in server_address;
    int server_len = sizeof(server_address);

    while(1) {

        socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

        if (socket_fd < 0) {
            error("Error creating socket");
        }


        bzero((char *) &server_address, server_len);
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = INADDR_ANY;
        port_number = atoi(argv[1]);
        server_address.sin_port = htons(port_number);

        if (bind(socket_fd, (struct sockaddr *) &server_address, server_len) < 0) {
            error("Error on binding");
        }

        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);

        struct sockaddr_in client_address;
        int client_len = sizeof(client_address);

        if (recvfrom(socket_fd, buffer, BUFFER_SIZE-1, 0, (struct sockaddr *) &client_address, (socklen_t *) &client_len) < 0) {
            error("Error reading from socket");
        }

        const int message_size = strlen(buffer) + 1;

        char message[message_size];
        string_to_uppercase(message, buffer);

        if (sendto(socket_fd, message, message_size, 0, (struct sockaddr *) &client_address, client_len) < 0) {
            error("Error sending message");
        }

        close(socket_fd);

        if (!strcmp(message, "QUIT"))  {
            break;
        }
    }

    return 0;



}