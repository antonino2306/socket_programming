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

void extract_numbers(char buffer[], unsigned int numbers[], size_t size) {

    char n1[size], n2[size];

    int i = 0;
    while(buffer[i] != ',') {
        n1[i] = buffer[i];
        i++;
    }
    n1[i] = '\0';
    i++;
    int j = 0;
    while(buffer[i] != '\0') {
        n2[j] = buffer[i];
        j++;
        i++;
    }
    buffer[j] = '\0';

    numbers[0] = atoi(n1);
    numbers[1] = atoi(n2);
}

int main(int argc, char *argv[]) {

    int sockfd, newsockfd;
    int portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[256];

    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("Error creating socket");
    }

    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*) &serv_addr, (socklen_t) sizeof(serv_addr)) < 0) {
        error("Error on binding");
    }

    listen(sockfd, 5);

    while(1) {

        clilen = sizeof(cli_addr);

        newsockfd = accept(sockfd, (struct sockaddr *) &serv_addr, (socklen_t *)&clilen);

        if (newsockfd < 0) {
            error("Error during connection with client");
        }

        pid_t pid = fork();

        if (pid == 0) {

            close(sockfd);

            bzero(buffer, 256);

            // formato messaggio di arrivo: num1,num2
            if (read(newsockfd, buffer, 256) < 0) {
                error("Error reading from socket");
            }

            unsigned int numbers[2];
            extract_numbers(buffer, numbers, 256);

            unsigned int sum = numbers[0] + numbers[1];
            bzero(buffer, 256);
            sprintf(buffer, "%d", sum);

            if (write(newsockfd, buffer, strlen(buffer)+1) < 0) {
                error("Error sending message");
            }

            close(newsockfd);
            return 0;

        }

        close(newsockfd);

    }

    return 0;

}