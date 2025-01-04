#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h> // tipi di dati di sistema
#include <sys/socket.h> // definizioni utili per le socket
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>


void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        fprintf(stderr, "Insert hostname and portnumber\n");
        exit(1);
    }

    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server; 
    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("Error opening socket");
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        error("Error getting server ip");
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[2]);
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr_list[0], (char*)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr*) &serv_addr, (socklen_t)sizeof(serv_addr)) < 0) {
        error("Error during connection");
    }

    int execute = 1;

    while(execute) {

        bzero(buffer, 256);

        printf("Enter message: ");
        scanf(" %255[^\n]", buffer);

        if (write(sockfd, buffer, strlen(buffer)+1) < 0) {
            error("Error on writing");
        }

        bzero(buffer, 256);
        if (read(sockfd, buffer, 255) < 0) {
            error("Error on reading");
        }

        printf("%s\n", buffer);

        if (strcmp(buffer, "QUIT") == 0) {
            execute = 0;
        }

    }

    close(sockfd);

}