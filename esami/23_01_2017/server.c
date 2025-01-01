#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h> // tipi di dati di sistema
#include <sys/socket.h> // definizioni utili per le socket
#include <netinet/in.h>


void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[]) {

    int sockfd, newsockfd;
    int portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    pid_t pid;

    if (argc < 3) {
        fprintf(stderr, "Insert port_number and name\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error opening socket");
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    portno = atoi(argv[1]);
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, (socklen_t)sizeof(serv_addr)) < 0) {
        error("Error on binding");
    }

    listen(sockfd, 5);

    printf("Server listening on port %s\n", argv[1]);

    while(1) {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);

        if (newsockfd < 0) {
            error("Error accepting connection from client");
        }

        pid = fork();

        if (pid == 0) {

            close(sockfd);

            bzero(buffer, 256);

            if (read(newsockfd, buffer, 255) < 0) {
                error("Error reading from socket");
            }

            printf("Lettura effettuata: %s\n", buffer);

            char * cli_msg = "Hello! My name is ";
            char * wrong_message = "Wrong message";

            if (strlen(buffer) <= strlen(cli_msg)) {

                if (write(newsockfd, wrong_message, strlen(wrong_message)+1) < 0) {
                    error("Error on writing");
                }

                return 0;

            }

            if (strncmp(buffer, cli_msg, strlen(cli_msg)) == 0) {

                if (buffer[strlen(buffer)-1] != '.') {
                    if (write(newsockfd, wrong_message, strlen(wrong_message)+1) < 0) {
                    error("Error on writing");
                }

                    return 0;
                }

                char * client_name = buffer + strlen(cli_msg);
                client_name[strlen(client_name)-1] = '\0';

                char response[256];
                bzero(response, 256);

                sprintf(response, "Hello %s! My name is %s", client_name, argv[2]);

                if (write(newsockfd, response, strlen(response)+1) < 0) {
                    error("Error writing on socket");
                }
            }


            close(newsockfd);
            return 0;

        }

        close(newsockfd);
    }

    close(sockfd);
    return 0;

}
