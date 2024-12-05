#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h> // tipi di dati di sistema
#include <sys/socket.h> // definizioni utili per le socket
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_SIZE 4

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {

    int socket_fd, port_number;
    struct hostent *server;
    struct sockaddr_in server_address;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s hostname port", argv[0]);
        exit(1);
    }

    port_number = atoi(argv[2]);
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        error("Error creating socket");
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        error("No such host");
    }

    bzero((char*)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_number);
    bcopy((char*) server->h_addr_list[0], (char*) &server_address.sin_addr.s_addr, server->h_length);

    printf("Connecting...\n");

    if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        error("Error on connection");
    }

    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);

    printf("Enter message: ");
    // devo lasciare uno spazio vuoto nel buffer per inserire il carattere di fine stringa
    scanf("%255[^\n]", buffer);
    printf("messaggio: %s\n", buffer);
    int number_of_bytes;

    number_of_bytes = write(socket_fd, buffer, BUFFER_SIZE);

    if (number_of_bytes < 0) {
        error("Error writing on socket");
    }

    bzero(buffer, BUFFER_SIZE);
    number_of_bytes = read(socket_fd, buffer, BUFFER_SIZE-1);

    if (number_of_bytes < 0) {
        error("Error reading from socket");
    }

    printf("%s\n", buffer);


}