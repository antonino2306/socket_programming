#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h> // tipi di dati di sistema
#include <sys/socket.h> // definizioni utili per le socket
#include <netinet/in.h>


void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Insert port number\n");
        exit(1);
    }

    int sockfd, newsockfd;
    int clilen, portno;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[256];

    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    portno = atoi(argv[1]);
    serv_addr.sin_port = htons(portno);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("Error opening socket");
    }

    if (bind(sockfd, (struct sockaddr *) &serv_addr, (socklen_t) sizeof(serv_addr)) < 0) {
        error("Error on binding");
    }

    listen(sockfd, 2);

    while (1) {

        clilen = sizeof(cli_addr);
        bzero((char*) &cli_addr, clilen);

        newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, (socklen_t *) &clilen);

        if (newsockfd < 0) {
            error("Error on accept");
        }

        pid_t pid = fork();

        if (pid == 0) {
            close(sockfd);
            int execute = 1;

            while (execute) {

                bzero(buffer, 256);

                if (read(newsockfd, buffer, 255) < 0) {
                    error("Error on reading");
                }

                if (write(newsockfd, buffer, strlen(buffer) + 1) < 0) {
                    error("Error writing on socket");
                }

                if (strcmp(buffer, "QUIT") == 0) {
                    execute = 0;
                }

            }

            printf("Closing connection with client\n");
            close(newsockfd);
            return 0;
        
        }

        close(newsockfd);
    }

    close(sockfd);
    return 0;
    
}