#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#define MAXLINE 256

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <server_name> <port>\n", argv[0]);
        exit(1);
    }

    int sockfd, portno;
    portno = atoi(argv[2]);
    char buffer[MAXLINE];
    struct sockaddr_in serv_addr;
    struct hostent * server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("Error opening socket");
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        error("Error");
    }

    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    bcopy((char*) server->h_addr_list[0], (char*) &serv_addr.sin_addr.s_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr*) &serv_addr, (socklen_t) sizeof(serv_addr)) < 0) {
        error("Error connecting to server");
    }

    while (1) {

        bzero(buffer, MAXLINE);

        if (read(sockfd, buffer, MAXLINE-1) < 0) {
            error("Error reading from socket");
        }

        if (strcmp(buffer, "1\n") == 0) {
            char choice[MAXLINE];
            printf("Fai la tua mossa: sasso, carta o forbice: ");
            scanf(" %255[^\n]", choice);

            if (write(sockfd, choice, strlen(choice) + 1) < 0) {
                error("Error sending message");
            }

        }
        else {
            
            printf("%s\n", buffer);

            if (strcmp(buffer, "Pareggio") != 0) {
                break;
            }

        }
        
    }

    close(sockfd);
   
}
