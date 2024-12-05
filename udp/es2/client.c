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

    int sockfd, portno;
    portno = atoi(argv[2]);
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;
    
    // Creazione della socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }
    
    struct hostent * server = gethostbyname(argv[1]);
    if (server == NULL) {
        error("Server not found");
    }

    // Configurazione dell'indirizzo del server
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portno);
    bcopy((char *) server->h_addr_list[0], (char *) &server_addr.sin_addr.s_addr, server->h_length);

    // Invio dei dati
    while (1) {
        bzero(buffer, BUFFER_SIZE);

        if (sendto(sockfd, "", 0, 0, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
            error("ERROR sending message");
        }

        struct sockaddr_in s_addr;
        int s_len = sizeof(s_addr);

        if (recvfrom(sockfd, buffer, 0, 0, (struct sockaddr *) &s_addr, &s_len) < 0) {
            error("Error reading");
        }

        printf("Connected to sever IP: %s port: %d\n", inet_ntoa(s_addr.sin_addr), ntohs(s_addr.sin_port));
        break;
    }

    close(sockfd);
    return 0;
}
