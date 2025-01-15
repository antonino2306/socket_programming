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

void my_echo(int sockfd, struct sockaddr *cli_addr, socklen_t clilen) {

    char msg[MAXLINE];

    if (recvfrom(sockfd, msg, MAXLINE-1, 0, cli_addr, (socklen_t*) &clilen) < 0) {
        error("Error reading from socket");
    }

    if (sendto(sockfd, msg, strlen(msg) + 1, 0, cli_addr, (socklen_t) clilen) < 0) {
        error("Error writing on socket");
    }

    close(sockfd);

}

int main(int argc, char *argv[]) {

    int sockfd;
    int portno;
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2) {
        fprintf(stderr, "Insert port number");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        error("Error opening socket");
    }

    if (bind(sockfd, (struct sockaddr*) &serv_addr, (socklen_t) sizeof(serv_addr)) < 0) {
        error("Error on binding");
    }

    my_echo(sockfd, (struct sockaddr*) &cli_addr, (socklen_t) sizeof(cli_addr));

}