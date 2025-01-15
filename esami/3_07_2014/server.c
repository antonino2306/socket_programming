#include <stdio.h>
#include <stdlib.h>     /* exit() */
#include <strings.h>
#include <string.h>    /* bzero(), bcopy() */
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>       /* time_t e ctime() */
#include <sys/types.h>  /* tipi di dati di sistema */
#include <sys/socket.h> /* definizioni utili per le socket() */

#define MAXLINE 4096

void error(char *msg) {
    perror(msg);
    exit(1);
}

long fibonacci(long n) {

    if (n == 1 ||  n == 2) {
        return 1;
    }
    
    return fibonacci(n-1) + fibonacci(n-2);
}

void send_fibonacci(int sockfd, struct sockaddr *cli_addr, socklen_t clilen) {

    int newsockfd;
    pid_t pid;
    char buffer[MAXLINE]; 

    while(1) {
        

        newsockfd = accept(sockfd, cli_addr, &clilen);

        if (newsockfd < 0) {
            error("Error connecting with client");
        }

        pid = fork();

        if (pid == 0) {
            close(sockfd);
            bzero(buffer, MAXLINE);

            if(read(newsockfd, buffer, MAXLINE-1) < 0) {
                error("Error reading from socket");
            }

            long n = atol(buffer);
            long fibonacci_n = fibonacci(n);

            bzero(buffer, MAXLINE);
            sprintf(buffer, "%ld", fibonacci_n);

            if (write(newsockfd, buffer, strlen(buffer)+1) < 0) {
                error("Error sending message");
            }

            close(newsockfd);
            exit(0);   
        }

        close(newsockfd);
    }

}

int main(int argc, char *argv[]) {

    // identificatori per la socket di benvenuto e la socket operativa
    int sockfd;
    int portno, clilen;
    char buffer[256];
    
    // struct per gli indirizzi delle socket
    struct sockaddr_in serv_addr, cli_addr;


    // È necessario passare il numero di porta da riga di comando
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("Error opening socket");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error on binding");
    }

    listen(sockfd, 5);

    send_fibonacci(sockfd, (struct sockaddr*) &cli_addr, (socklen_t) clilen);
    return 0;

}