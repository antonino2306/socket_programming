#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h> // tipi di dati di sistema
#include <sys/socket.h> // definizioni utili per le socket
#include <netinet/in.h>

#define N_CLIENT 2

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[]) {

        // identificatori per la socket di benvenuto e la socket operativa
    int sockfd, newsockfd[N_CLIENT];
    int portno, clilen;
    char buffer[256];
    
    // struct per gli indirizzi delle socket
    struct sockaddr_in serv_addr, cli_addr[N_CLIENT];
    pid_t pid;

    // È necessario passare il numero di porta da riga di comando
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("Error opening socket");
    }

    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error on binding");
    }

    listen(sockfd, N_CLIENT);

    while (1) {

        clilen = sizeof(cli_addr[0]);

        for (int i = 0; i < N_CLIENT; i++) {

            newsockfd[i] = accept(sockfd, (struct sockaddr*) &cli_addr[i], (socklen_t*) &clilen);
            if (newsockfd[i] < 0) {
                error("Error on accept");
            }
            printf("client[%d] connesso\n", i);

        }

        pid = fork();


        
        if (pid == 0) {
            close(sockfd);
            for (int i = 0; i < N_CLIENT; i++) {
                sleep(1);
                char auth_message[3] = "1\n";

                if (write(newsockfd[i], auth_message, strlen(auth_message)+1) < 0) {
                    error("Error on writing");
                }
                printf("Autorizzazione a scrivere inviata a client[%d]\n", i);

                bzero(buffer, 256);

                if (read(newsockfd[i], buffer, 255) < 0) {
                    error("Error reading from socket");
                }

                printf("messaggio ricevuto da client[%d]: %s\n", i, buffer);


                for (int j = 0; j < N_CLIENT; j++) {

                    if (j != i) {

                        if (write(newsockfd[j], buffer, strlen(buffer)+1) < 0) {
                            error("Error on sending message. Closing connection");
                        }
                    }

                }

            }

            for (int i = 0; i < N_CLIENT; i++) {
                close(newsockfd[i]);
            }

            return 0;
        }

        for (int i = 0; i < N_CLIENT; i++) {
            close(newsockfd[i]);
        }

    }

    close(sockfd);
    return 0;

}