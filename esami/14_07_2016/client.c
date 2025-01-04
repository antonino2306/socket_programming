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
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("Error opening socket");
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }

    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[2]);
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr_list[0], (char*) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERRROR connecting");
    }


    while(1) {
        bzero(buffer, 256);

        printf("Aspettando messaggio da server...\n");
        
        int n = read(sockfd, buffer, 256);
        
        if (n< 0) {
            error("Error reading from socket");
        }

        if (strcmp(buffer, "1\n") == 0) {
            char message[256];
            bzero(message, 256);
            printf("Insert message: ");
            scanf(" %255[^\n]", message);
            printf("Il messaggio da inviare è: %s\n", message);

            int n = write(sockfd, message, strlen(message) + 1);

            if (n < 0) {
                error("Error writing on socket");
            }

            printf("Messaggio inviato");
        }
        else {
            printf("%s\n", buffer);
        }

    }

    close(sockfd);


}