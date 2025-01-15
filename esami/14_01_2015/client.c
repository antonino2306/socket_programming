#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h> // tipi di dati di sistema
#include <sys/socket.h> // definizioni utili per le socket
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_SIZE 256

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[]) {

    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent* server;
    char buffer[BUFFER_SIZE];
    unsigned int n1, n2;

    if (argc < 3) {
        error("Insert server name and port number");
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("Error opening socket");
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        error("Error");
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[2]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    bcopy((char*)server->h_addr_list[0], (char*)&serv_addr.sin_addr.s_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr*) &serv_addr, (socklen_t) sizeof(serv_addr)) < 0) {
        error("Error during connection");
    }

    printf("Inserisci il primo numero: ");
    scanf(" %u", &n1);
    printf("Inserisci il secondo numero: ");
    scanf(" %u", &n2);

    bzero(buffer, BUFFER_SIZE);
    sprintf(buffer, "%u,%u", n1, n2);

    if (write(sockfd, buffer, strlen(buffer) + 1) < 0) {
        error("Error writing on socket");
    }

    bzero(buffer, BUFFER_SIZE);
    if (read(sockfd, buffer, BUFFER_SIZE) < 0) {
        error("Error on reading");
    }

    printf("Somma: %s\n", buffer);
    close(sockfd);

}