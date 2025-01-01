/* Esempio di client UDP
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     /* per strlen() */
#include <strings.h>    /* per bzero() e bcopy() */

#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>  /* tipi di dati di sistema */
#include <sys/socket.h> /* definizioni utili per le socket() */

#define MAXLINE 4096

/* Funzione di utilita' per la stampa di errori critici
 */
void error(char *msg) {
    perror(msg);
    exit(1);
}

/* main: prende come argomenti lostname e la porta su cui connettersi
 */
int main(int argc, char *argv[]) {
	int sockfd;
	int portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server; /* struttura dati per rappresentare gli host (nome, alias, indirizzo)*/
	char buffer[256];

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
	
	portno = atoi(argv[2]);	
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno); 
		/* host-to-network short: converte un intero in un formato a 16 bit, indipendente dall'artchitettura
		 big indian o little indian dell'host */

	int n_try = 0;

    while (n_try < 3) {
        int number;

        printf("Inserisci un numero: ");
        scanf(" %d", &number);
        bzero(buffer, 256);
        sprintf(buffer, "%d", number);

        if (sendto(sockfd, buffer, strlen(buffer)+1, 0, (struct sockaddr *)&serv_addr, (socklen_t)sizeof(serv_addr)) < 0) {
            error("Error on send");
        }

        if (recvfrom(sockfd, buffer, 255, 0, NULL, NULL) < 0) {
            error("Error on receive");
        }

        if (!strcmp(buffer, "Hai vinto")) {
            printf("%s\n", buffer);
            break;
        }

        n_try++;
        printf("%s\n", buffer);

    }
    close(sockfd);
	return 0;
}
