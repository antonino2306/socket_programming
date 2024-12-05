#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h> // tipi di dati di sistema
#include <sys/socket.h> // definizioni utili per le socket
#include <netinet/in.h>
#include <ctype.h>

#define BUFFER_SIZE 4

void error(char *msg) {
    perror(msg);
    exit(1);
}

void string_to_uppercase(char *dest, char *src) {

    int i;
    for (i = 0; src[i] != '\0'; i++) {
        dest[i] = toupper(src[i]);
    }
    dest[i] = '\0';
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "No port provided\n");
        exit(1);
    }

    int welcome_socket_fd, port_number;
    struct sockaddr_in server_address;

    welcome_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    printf("Creating socket...\n");
    if (welcome_socket_fd < 0) {
        error("Error creating socket");
    }

    bzero((char*)&server_address, sizeof(server_address));
    port_number = atoi(argv[1]);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port_number);

    if (bind(welcome_socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        error("Error on binding");
    }

    printf("Binding successful on port %d\n", port_number);

    listen(welcome_socket_fd, 5);

    int execute = 1;

    int socket_fd;
    struct sockaddr_in client_address;
    char buffer[BUFFER_SIZE];
    
    while (execute) {

        int client_address_len = sizeof(client_address);

        socket_fd = accept(welcome_socket_fd, (struct sockaddr *)&client_address, (socklen_t *) &client_address_len);

        if (socket_fd < 0) {
            error("Error on accept client connection");
        }
        printf("Connection accepted\n");

        pid_t pid = fork();

        if (pid == 0) {

            close(welcome_socket_fd);
            bzero(buffer, BUFFER_SIZE);

            int number_of_bytes;
            // nella read devo mettere buffer size -1 perché devo essere sicuro che venga lasciato uno spazio per il carattere di fine stringa
            // Se non lascio questo spazio il carattere \0 viene memorizzato nel byte successivo che non è allocato per la stringa e quindi potrebbe essere riscritto
            number_of_bytes = read(socket_fd, buffer, BUFFER_SIZE-1);

            if (number_of_bytes < 0) {
                error("ERROR reading from socket");
            }
            printf("Il messaggio in arrivo è: %s\n", buffer);

            char message[BUFFER_SIZE];
            bzero(message, BUFFER_SIZE);
            string_to_uppercase(message, buffer);

            number_of_bytes = write(socket_fd, message, BUFFER_SIZE);

            if (number_of_bytes < 0) {
                error("Error writing on socket");
            }

            close(socket_fd);
            return 0;
        }


        close(socket_fd);
    }
    return 0;

}