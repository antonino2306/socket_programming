#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(char *msg) {
    perror(msg);
    exit(1);
}

void get_ip_address(char *hostname, char *ip) {
    srand(time(NULL));
    int ip_digits[4];

    for (int i = 0; i < 4; i++) {
        ip_digits[i] = rand() % 255;
    }

    sprintf(ip, "%d.%d.%d.%d", ip_digits[0], ip_digits[1], ip_digits[2], ip_digits[3]);

}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        error("Insert port number");
    }

    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    int clilen, portno;
    char recvbuffer[256], sendbuffer[256];


    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    portno = atoi(argv[1]);
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        error("Error opening socket");
    }

    if (bind(sockfd, (struct sockaddr *) &serv_addr, (socklen_t) sizeof(serv_addr)) < 0) {
        error("Error on binding");
    }


    clilen = sizeof(cli_addr);
    bzero(recvbuffer, 256);
    if (recvfrom(sockfd, recvbuffer, 255, 0, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen) < 0) {
        error("Error reading data from socket");
    }
    bzero(sendbuffer, 256);
    get_ip_address(recvbuffer, sendbuffer);

    if (sendto(sockfd, sendbuffer, strlen(sendbuffer) + 1, 0, (struct sockaddr *) &cli_addr, (socklen_t) clilen) < 0) {
        error("Error sending message");
    }

    close(sockfd);

    return 0;

    

}