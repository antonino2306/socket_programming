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

    int n_try = 0;
    while (n_try < 3) {

        bzero(buffer, 256);
        char psw[256];
        bzero(psw, 256);
        printf("Enter password: ");
        scanf(" %255[^\n]", psw);

        if (write(sockfd, psw, strlen(psw)+1) < 0) {
            error("Error on writing");
        }

        if (read(sockfd, buffer, 255) < 0) {
            error("Error on reading");
        }

        if (strcmp(buffer, "wrong password") != 0) {
            printf("%s\n", buffer);
            break;
        }

        printf("Wrong password\n");
        n_try++;

    }

    close(sockfd);
    return 0;
}