#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h> // tipi di dati di sistema
#include <sys/socket.h> // definizioni utili per le socket
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>


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
        exit(1);
    }

    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr_list[0], (char*) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    printf("Connecting...\n");
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERRROR connecting");
    }

    printf("Enter the position on the number in the fibonacci sequence: ");

    bzero(buffer, 256);
    fgets(buffer, 255, stdin);

    n = atoi(buffer);

    if (write(sockfd, buffer, strlen(buffer)) < 0) {
        error("ERROR writing to socket");
    }

    bzero(buffer, 256);

    if (read(sockfd, buffer, 255) < 0) {
        error("ERROR reading from socket");
    }

    printf("Il numero in posizione %d nella sequenza di fibonacci è %s\n", n, buffer);
    close(sockfd);

    return 0;
}