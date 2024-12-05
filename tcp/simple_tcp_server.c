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

int main(int argc, char *argv[]) {
    
    // identificatori per la socket di benvenuto e la socket operativa
    int sockfd, newsockfd;
    int portno, clilen;
    char buffer[256];
    
    // struct per gli indirizzi delle socket
    struct sockaddr_in serv_addr, cli_addr;
    int n, counter;
    pid_t pid;

    counter = 0;

    // È necessario passare il numero di porta da riga di comando
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    printf("Creazione socket\n");

    // Creazione della socket di benvenuto
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error opening socket");
    }
    printf("Id socket: %d\n", sockfd);

    // Inizializzazione della socket
    // Definizione dell'indirizzo

    // inizializzo tutti i byte della struct a zero per eliminare i garbage values
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error in binding");
    }
    printf("Bind successful on port %d\n", portno);

    // Coda di attesa che può contenere al massimo 5 richieste
    listen(sockfd, 5);

    do {
        clilen = sizeof(cli_addr);
        // Il secondo parametro è l'indirizzo di memoria della struct che contiente l'indirizzo della socket
        // Il terzo parametro è la dimensione in byte della struct
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen);

        if (newsockfd < 0) {
            error("Error on accept");
        }

        // zero se è il processo figlio, maggiore di zero se è nel processo padre
        pid = fork();

        if (pid == 0) {
            // il processo figlio chiude la socket di benvenuto
            close(sockfd);
            
            bzero(buffer, 256);
            // n è il numero di byte letti dalla socket
            n = read(newsockfd, buffer, 255);

            if (n < 0) {
                error("ERROR reading from socket");
            }

            if (!strcmp(buffer, "T\n")) {
                counter++;
                printf("%d\n", counter);
                time_t t = time(NULL);

                char *timestr = ctime(&t);
                // il +1 serve ad includere il carattere di fine scringa
                n = write(newsockfd, timestr, strlen(timestr)+1);

                if (n < 0) {
                    error("ERROR writing on socket");
                }
            }

            else if (!strcmp(buffer, "N\n")) {
                // counter è di tipo int (4 byte) quidi cnt ha dimensione 5 perché devo includere il carattere di fine stringa
                char cnt[5];
                sprintf(cnt, "%d", counter);

                n = write(newsockfd, cnt, strlen(cnt));

                if (n < 0) {
                    error("ERROR writing to socket");
                }
            }

            else {
                printf("Please press a key: ");
                char c;
                scanf("%c", &c);
                n = write(newsockfd, "Message received\n", 18);

                if (n < 0) {
                    error("ERRROR writing to socket");
                }
            }

            close(newsockfd);
            return 0;
        }

        close(newsockfd);
    } while(1);
    return 0;



}
