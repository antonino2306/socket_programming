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
#define N_CLIENT 3

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        error("No port provided");
    }

    int welcome_socket_fd, port_number, socket_fd[N_CLIENT];
    struct sockaddr_in server_address, client_address[N_CLIENT];
    int client_len = sizeof(client_address[0]);

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
    listen(welcome_socket_fd, N_CLIENT);

    // se non metto questo while il processo padre non torna in ascolto sulla socket di benvenuto e si blocca tutto
    while (1) {

        bzero((char *) &client_address, N_CLIENT*client_len);
        
        for (int i = 0; i < N_CLIENT; i++) {
            socket_fd[i] = accept(welcome_socket_fd, (struct sockaddr *)&client_address[i], &client_len);

            if (socket_fd[i] < 0) {
                error("Error on accept connection from client");
            }
            // ad ogni connessione creata invia al client un messaggio con il suo turno trasmissivo
            char turn[2];
            sprintf(turn, "%d", i); 
            if (write(socket_fd[i], turn, 2) < 0) {
                error("Error on writing");
            }

        }

        pid_t pid = fork();
        
        if (pid == 0) {

            close(welcome_socket_fd);
            char buffer[BUFFER_SIZE];
            int execute = 1;
            
            int client_turn = 0;
            while (execute)
            {
                bzero(buffer, BUFFER_SIZE);
                // leggendo BUFFER_SIZE bytes sono sicuro di leggere dalla socket anche il carattere terminatore di stringa
                if (read(socket_fd[client_turn], buffer, BUFFER_SIZE) < 0) {
                    for (int i = 0; i < N_CLIENT; i++) {
                        close(socket_fd[i]);
                    }
                    error("Error reading message. Closing connection");
                }

                printf("In arrivo da client[%d]: %s\n", client_turn, buffer);

                for (int i = 0; i < N_CLIENT; i++) {

                    if (i != client_turn) {

                        if (write(socket_fd[i], buffer, strlen(buffer)+1) < 0) {
                            for (int i = 0; i < N_CLIENT; i++) {
                                close(socket_fd[i]);
                            }
                            error("Error on sending message. Closing connection");
                        }
                    }

                }
                
                if (!strcasecmp(buffer, "quit")) {
                    printf("Ending conversation. Closing connection\n");
                    execute = 0;
                    continue;
                }
                
                client_turn++;
                // appena finisce il giro di invio messaggi lo faccio ripartire
                if (client_turn == N_CLIENT) {
                    client_turn = 0;
                }
                

            }

            for (int i = 0; i < N_CLIENT; i++) {
                close(socket_fd[i]);
            }
            return 0;
        } 
        else if (pid > 0) {
            for (int i = 0; i < N_CLIENT; i++) {
                close(socket_fd[i]);
            }
        }
        else {
            error("Error on fork");
        }

    }

    // chiudo la socket di benvenuto nel padre
    close(welcome_socket_fd);
    return 0;

}