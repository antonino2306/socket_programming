#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h> // tipi di dati di sistema
#include <sys/socket.h> // definizioni utili per le socket
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


void error(char *msg) {
    perror(msg);
    exit(1);
}


int main(int argc, char *argv[]) {
    
    // identificatori per la socket di benvenuto e la socket operativa
    int sockfd;
    int portno, clilen;
    char buffer[256];
    // struct per gli indirizzi delle socket
    struct sockaddr_in serv_addr, cli_addr;
    unsigned int random_number = rand() % 5;


    if (argc < 2) {
        error("Provide port number");
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        error("Error opening socket");
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));


    if (bind(sockfd, (struct sockaddr *)&serv_addr, (socklen_t)sizeof(serv_addr)) < 0) {
        error("Error on binding");
    }

    int n_try = 0;
    clilen = sizeof(cli_addr);
    while (n_try < 3) {
    

        if (recvfrom(sockfd, buffer, 255, 0, (struct sockaddr *)&cli_addr, (socklen_t*)&clilen) < 0) {
            error("Error on recive");
        }

        if (atoi(buffer) == random_number) {
            char * msg = "Hai vinto";
            if (sendto(sockfd, msg, strlen(msg)+1, 0, (struct sockaddr*)&cli_addr, clilen) < 0) {
                error("Error sending messagge");
            }
            break;
        }
        else  {
            n_try++;
            if (n_try == 3) {
                char * msg = "Hai perso";
                if (sendto(sockfd, msg, strlen(msg)+1, 0, (struct sockaddr*)&cli_addr, clilen) < 0) {
                    error("Error sending messagge");
                }
            }

            char * msg = "Riprova";
            if (sendto(sockfd, msg, strlen(msg)+1, 0, (struct sockaddr*)&cli_addr, clilen) < 0) {
                error("Error sending messagge");
            }

        }
        

    }

    close(sockfd);
    return 0;

}