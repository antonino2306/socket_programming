#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h> // tipi di dati di sistema
#include <sys/socket.h> // definizioni utili per le socket
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

void error(char *msg) {
    perror(msg);
    exit(1);
}


int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(0);
    }

    printf("Opening socket\n");
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    printf("Socket opened\n");
    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr_list[0], (char*) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    printf("Connecting...\n");
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERRROR connecting");
    }

    struct sockaddr_in local_address;
    socklen_t address_len = sizeof(local_address);
    
    if (getsockname(sockfd, (struct sockaddr *) &local_address, &address_len) < 0) {
        error("Error retrieving socket info");
    }

    char ip_address[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &local_address.sin_addr, ip_address, sizeof(ip_address));

    printf("Client info: IP: %s port: %d\n", ip_address, ntohs(local_address.sin_port));


    bzero(buffer, 256);
    sprintf(buffer, "Connection from client IP: %s port: %d\n", ip_address, ntohs(local_address.sin_port));

    // printf("dim buffer: %d\n", strlen(buffer));
    
    // devo mettere +1 per essere sicuro che venga preso anche il carattere terminatore \0 (chiedere al professore)
    n = write(sockfd, buffer, strlen(buffer)+1);

    if (n < 0) {
        error("ERROR writing to socket");
    }

    bzero(buffer, 256);
    n = read(sockfd, buffer, 255);

    if (n < 0) {
        error("ERROR reading from socket");
    }

    printf("%s", buffer);
    close(sockfd);

    return 0;
}