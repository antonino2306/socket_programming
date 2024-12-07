#include <stdio.h>

#include <stdlib.h>     /* exit() */
#include <strings.h>    /* bzero(), bcopy() */

#include <unistd.h>
#include <netinet/in.h>
#include <time.h>       /* time_t e ctime() */
#include <sys/types.h>  /* tipi di dati di sistema */
#include <sys/socket.h> /* definizioni utili per le socket() */

#define MAXLINE 4096

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
	int sockfd;
    int portno, clilen;
    int bytesread, counter;
	struct sockaddr_in serv_addr, cli_addr;
	char buffer[MAXLINE], readLine[MAXLINE];


	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

	/*Inizializzazione dei parametri della socket*/
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	/* Creazione della socket:
	 AF_INET indica la famiglia di protocolli usati da Internet;
	 SOCK_DGRAM indica una socket non orientata alla connessione;
	 0 e' l'identificativo del protocollo (con SOCK_DGRAM e' UDP).*/
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
    	error("ERROR opening socket");
	}

	/*Creazione della socket*/
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
 		error("ERROR on binding");
	}

	clilen = sizeof(cli_addr);
	counter = 0;
	for (;;) {
		printf("ciao: %d\n", counter);
 		bzero(buffer, MAXLINE);
		/*Ricezione di un pacchetto dalla socket.
		 Parametri di recvfrom:
		 sockfd: descrittore della socket (input);
		 buffer: buffer per la memorizzazione dei dati da ricevere (output);
		 MAXLINE: Dimensione del buffer (input);
		 0: flags (input)
		 (struct sockaddr *)&cli_addr: puntatore alla zona di memoria dove e' memorizzato l'indirizzo del mittente (output)
		 clilen: dimensione della struttura dati per la memorizzazione dell'indirizzo (input)
		 */
 		bytesread = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
 		printf("check\n");
 		if(!strcmp(buffer, "T\n")) {
          counter++;
		  /* Ora corrente del sistema */
          time_t t = time(NULL);
          char* timestr = ctime(&t);
		  /* Invio dati dalla socket:
		   sockfd: descrittore della socket; (input)
		   timestr: buffer contenente i dati da inviare (input)
		   strlen(timestr)+1: dimensione dei dati da inviare (input)
		   0: flags (input)
		   (struct sockaddr *)&cli_addr: puntatore alla zona di memoria dove e' memorizzato l'indirizzo del destinatario
		   clilen: dimensione della struttura dati per la memorizzazione dell'indirizzo (input)
		   */
          sendto(sockfd, timestr, strlen(timestr)+1, 0, (struct sockaddr *)&cli_addr, clilen);
        }else if(!strcmp(buffer, "N\n")) {
          char cnt[5];
          sprintf(cnt, "%d\n", counter);
          sendto(sockfd, cnt, strlen(cnt)+1, 0, (struct sockaddr *)&cli_addr, clilen);
        } else {
          // delay
          printf("Premi un tasto...");
          fgets(readLine, MAXLINE, stdin);
          sendto(sockfd, "ricevuto\n", 11, 0, (struct sockaddr *)&cli_addr, clilen);
        }
 	}
}
