#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 256

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {

    if (argc < 1) {
        error("No port provided");
    }

    int socket_fd, port_number;
    struct sockaddr_in server_address, client_address;
    char buffer[BUFFER_SIZE];

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_fd < 0) {
        error("Error creating socket");
    }

    printf("Socket creata\n");
    bzero((char*) &server_address, sizeof(server_address));
    port_number = atoi(argv[1]);
    
    server_address.sin_port = htons(port_number);
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_family = AF_INET;

    if (bind(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        error("Error on binding");
    }

    printf("Binding effettuato\n");
    while (1) {

        int bytes_read;
        bzero(buffer, BUFFER_SIZE);

        int client_address_size = sizeof(client_address);
        bzero((char*)&client_address, client_address_size);
        
        // memorizzo in apposite variabili le informazioni relative al client
        bytes_read = recvfrom(socket_fd, buffer, BUFFER_SIZE-1, 0, (struct sockaddr *) &client_address, (socklen_t *)&client_address_size);

        if (bytes_read < 0) {
            error("Error on reading");
        }

        printf("Dati arrivati\n");
        // if (!strcasecmp(buffer, "quit\n")) {
        //     break;
        // }

        bytes_read = sendto(socket_fd, "ciao", 4, 0, (struct sockaddr *)&client_address, client_address_size);

        if (bytes_read < 0) {
            error("Error writing");
        }
        printf("Connection from client IP: %s port: %d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        break;
    }
    
    close(socket_fd);


}
