#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>       /* funzioni time() e ctime() */
#include <unistd.h>
#include <sys/types.h>  /* tipi di dati di sistema */
#include <sys/socket.h> /* definizioni utili per le socket() */
#include <netinet/in.h>

#define BUFFER_SIZE 256
#define N_CLIENT 2

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Insert port number");
        exit(1);
    }

    int sockfd, newsockfd[N_CLIENT];
    int clilen, portno;
    struct sockaddr_in serv_addr, cli_addr[N_CLIENT];
    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("Error opening socket");
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, (socklen_t) sizeof(serv_addr)) < 0) {
        error("Error on binding");
    }

    printf("Server waiting for connections on port: %s\n", argv[1]);

    listen(sockfd, N_CLIENT);

    while(1) {
        clilen = sizeof(cli_addr[0]);
        bzero((char *) &cli_addr, N_CLIENT*clilen);

        for (int i = 0; i < N_CLIENT; i++) {
            newsockfd[i] = accept(sockfd, (struct sockaddr *) &cli_addr[i], (socklen_t *) &clilen);

            if (newsockfd[i] < 0) {
                error("Error during connection with client");
            }
        } 

        int trasmission_turn = 0;
        pid_t pid = fork();

        if (pid == 0) {

            close(sockfd);

            // do {

                do {
                    bzero(buffer, BUFFER_SIZE);
                    printf("Trasmission turn: %d\n", trasmission_turn);
                    char *auth = "1\n";
                    int n = write(newsockfd[trasmission_turn], auth, strlen(auth)+1);


                    if (n < 0) {
                        error("Error on writing");
                    }

                    printf("Ho inviato %d byte\n", n);

                    if (read(newsockfd[trasmission_turn], buffer, BUFFER_SIZE) < 0) {
                        error("Error on reading");
                    }

                    for (int i = 0; i < N_CLIENT; i++) {

                        if (i != trasmission_turn) {
                            if (write(newsockfd[i], buffer, strlen(buffer)+1) < 0) {
                                error("Error sending message");
                            }
                        }

                    }

                    trasmission_turn++;
                    if (trasmission_turn == N_CLIENT) {
                        trasmission_turn = 0;
                    }


                } while (strcmp(buffer, "QUIT") != 0);


            // } while (strcmp(buffer, "QUIT") != 0);
            
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