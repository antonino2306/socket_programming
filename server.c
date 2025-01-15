#include <stdio.h>
#include <stdlib.h>     /* exit() */
#include <strings.h>    /* bzero(), bcopy() */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>       /* time_t e ctime() */
#include <sys/types.h>  /* tipi di dati di sistema */
#include <sys/socket.h> /* definizioni utili per le socket() */
#include <arpa/inet.h>

#define MAXLINE 4096
#define N_CLIENT 2

void error(char *msg) {
    perror(msg);
    exit(1);
}

// ritorna -1 per pareggio; 0 per vittoria primo client; 1 per vittoria secondo client
int play(char g1[], char g2[]) {

    if (strcmp(g1, g2) == 0) {
        return -1;
    }

    if (strcasecmp(g1, "carta") == 0) {
    
        if (strcasecmp(g2, "sasso") == 0) {
            return 0;
        }

        if (strcasecmp(g2, "forbice") == 0) {
            return 1;
        }
    
    }

    if (strcasecmp(g1, "sasso") == 0) {
    
        if (strcasecmp(g2, "forbice") == 0) {
            return 0;
        }

        if (strcasecmp(g2, "carta") == 0) {
            return 1;
        }
    
    }

    if (strcasecmp(g1, "forbice") == 0) {
    
        if (strcasecmp(g2, "carta") == 0) {
            return 0;
        }

        if (strcasecmp(g2, "sasso") == 0) {
            return 1;
        }
    
    }

    return -1;
}


int main(int argc, char* argv[]) {

    int sockfd, portno, clilen, newsockfd[N_CLIENT];
    struct sockaddr_in serv_addr, cli_addr[N_CLIENT];
    char buffer1[MAXLINE], buffer2[MAXLINE];
    pid_t pid;

    if (argc < 2) {
        error("Insert port number");
    }  

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("Error opening socket");
    }

    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error on binding");
    }

    listen(sockfd, 2);

    while(1) {
        clilen = sizeof(cli_addr[0]);
        
        for (int i = 0; i < N_CLIENT; i++) {
            newsockfd[i] = accept(sockfd, (struct sockaddr*) &cli_addr[i], (socklen_t*) &clilen);

            if (newsockfd[i] < 0) {
                error("Error opening socket");
            }
        }

        pid = fork();

        if (pid == 0) {
            close(sockfd);

            char msg1[] = "Hai vinto";
            char msg2[] = "Hai perso";
            char msg3[] = "Pareggio";
            int winner = -1;

            while (winner == -1) {

                bzero(buffer1, MAXLINE);
                bzero(buffer2, MAXLINE);

                if (write(newsockfd[0], "1\n", 3) < 0) {
                    error("Error on writing");
                }

                if (read(newsockfd[0], buffer1, MAXLINE-1) < 0) {
                    error("Error on reading");
                } 

                if (write(newsockfd[1], "1\n", 3) < 0) {
                    error("Error on writing");
                }

                if (read(newsockfd[1], buffer2, MAXLINE-1) < 0) {
                    error("Error on reading");
                } 

                // ritorna 0 se vince client1; 1 se vince client2; -1 se pareggiano
                winner = play(buffer1, buffer2);
                printf("Esito: %d\n", winner);

                if (winner == -1) {
                    
                    for (int i = 0; i < N_CLIENT; i++) {
                        if (write(newsockfd[i], msg3, strlen(msg3) + 1) < 0) {
                            error("Error writing on socket");
                        }
                    }

                }
                else {

                    if (write(newsockfd[winner], msg1, strlen(msg1)+1) < 0) {
                        error("Error writing on socket");
                    }


                    if (write(newsockfd[!winner], msg2, strlen(msg2)+1) < 0) {
                        error("Error writing on socket");
                    }

                }

            }

            close(newsockfd[0]);
            close(newsockfd[1]);
            return 0;

        }

        close(newsockfd[0]);
        close(newsockfd[1]);
    }

    close(sockfd);
    return 0;
 
}