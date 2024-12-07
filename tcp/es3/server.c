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


void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        error("No port provided");
    }

    int welcome_socket_fd, port_number;
    struct sockaddr_in server_address;

    welcome_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (welcome_socket_fd < 0) {
        error("Error opening socket");
    }

    bzero((char *) &server_address, sizeof(server_address));
    port_number = atoi(argv[1]);
    server_address.sin_port = htons(port_number);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(welcome_socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        error("Error on binding");
    } 

    printf("Server waiting for connection on port: %d\n", port_number);
    listen(welcome_socket_fd, 0);

    // se non metto questo while il processo padre non torna in ascolto sulla socket di benvenuto e si blocca tutto
    while (1) {

        struct sockaddr_in client_address;
        int client_len, socket_fd;


        client_len = sizeof(client_address);
        bzero((char *) &client_address, client_len),
        
        socket_fd = accept(welcome_socket_fd, (struct sockaddr *)&client_address, &client_len);
        
        if (socket_fd < 0) {
            error("Error on accept connection from client");
        }

        pid_t pid = fork();
        
        if (pid == 0) {

            close(welcome_socket_fd);
            char buffer[BUFFER_SIZE];
            int execute = 1;
            
            while (execute)
            {
                bzero(buffer, BUFFER_SIZE);

                // leggendo BUFFER_SIZE bytes sono sicuro di leggere dalla socket anche il carattere terminatore di stringa
                if (read(socket_fd, buffer, BUFFER_SIZE) < 0) {
                    close(socket_fd);
                    error("Error reading message. Closing connection");
                }

                printf("Client: %s\n", buffer);

                if (!strcasecmp(buffer, "quit")) {
                    printf("Ending conversation. Closing connection\n");
                    execute = 0;
                    continue;
                }
            

                printf("Enter message: ");
                bzero(buffer, BUFFER_SIZE);
                
                scanf(" %255[^\n]", buffer);

                // Scrivo solo i byte del messaggio + il terminatore di stringa 
                if (write(socket_fd, buffer, strlen(buffer)+1) < 0) {
                    close(socket_fd);
                    error("Error on sending message. Closing connection");
                }

                

            }

            close(socket_fd);
            return 0;
        } 
        else if (pid > 0) {
            close(socket_fd);
        }
        else {
            error("Error on fork");
        }

    }

    // chiudo la socket di benvenuto nel padre
    close(welcome_socket_fd);
    return 0;

}