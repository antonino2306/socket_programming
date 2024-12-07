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

int main(int argc, char *argv[]) {

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    int socket_fd, port_number;
    struct sockaddr_in server_address;
    struct hostent *server;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        error("Error opening socket");
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        error("Host not found");
    }

    bzero((char *)&server_address, sizeof(server_address));
    port_number = atoi(argv[2]);
    server_address.sin_port = htons(port_number);
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], (char *) &server_address.sin_addr.s_addr, server->h_length);

    if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        error("Error connecting to server");
    }

    char buffer[BUFFER_SIZE];

    while (1)
    {
        bzero(buffer, BUFFER_SIZE);

        // todo ricezione del messaggio con il turno trasimissivo


        // se è il tuo turno scrivi altrimenti rimani in ricezione
        printf("Enter message: ");
        scanf(" %255[^\n]", buffer);

        if (write(socket_fd, buffer, strlen(buffer)+1) < 0) {
            close(socket_fd);
            error("Error sending message. Closing connection");
        }

        if (!strcasecmp(buffer, "quit")) {
            printf("Ending conversation. Closing connection\n");
            break;
        }

        bzero(buffer, BUFFER_SIZE);
        
        if (read(socket_fd, buffer, BUFFER_SIZE) < 0) {
            close(socket_fd);
            error("Error reading message. Closing connection");
        }

        printf("From server: %s\n", buffer);


    }
    
    close(socket_fd);

}