#include <stdio.h>
#include <stdlib.h> /* exit() */
#include <string.h>    
#include <strings.h>    /* bzero(), bcopy() */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define BUFFER_SIZE 256

void error(char *msg) {
    perror(msg);
    exit(1);
}


int main(int argc, char* argv[]) {

    if (argc < 3) {
        fprintf(stderr, "Insert hostname and port number");
        exit(1);
    }

    int sockfd;
    int portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[BUFFER_SIZE];


    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("Error opening socket");
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        error("Error in dns service");
    }

    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[2]);
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr_list[0], (char*) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);


    if (connect(sockfd, (struct sockaddr*) &serv_addr, (socklen_t)sizeof(serv_addr)) < 0) {
        error("Error during connection");
    }

    do {

        bzero(buffer, BUFFER_SIZE);
        
        printf("Sto per leggere dalla socket:\n");

        int n = read(sockfd, buffer, BUFFER_SIZE-1);
        if (n < 0) {
            error("Error on reading");
        }
        printf("Il messaggio letto: %s è grande %d byte\n", buffer, n);


        if (strcmp(buffer, "1\n") == 0) {

            printf("Scrivi messaggio: ");
            // in questo modo la lettura si ferma quando viene premuto invio e il carattere \n non viene inserito nel buffer
            // quindi scrivo 1 e premo invio nel buffer viene inserito solo 1 e non si verificano problemi con i turni di trasmissione
            char buffer2[256];
            bzero(buffer2, BUFFER_SIZE);
            scanf(" %255[^\n]", buffer2);

            if (write(sockfd, buffer2, strlen(buffer2)+1) < 0) {
                error("Error on writing");
            }

        }
        else {
            printf("%s\n", buffer);
        }

        
    } while(strcmp(buffer, "QUIT") != 0);

    close(sockfd);
    return 0;

}