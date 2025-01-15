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
	
	char sendline[MAXLINE], recvline[MAXLINE+1];
	
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

    
    char buffer[MAXLINE];
    bzero(buffer, MAXLINE);
    printf("Inserisci messaggio: ");
    scanf(" %255[^\n]", buffer);


    if (sendto(sockfd, buffer, strlen(buffer) + 1, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error sending message");
    }

    bzero(buffer, MAXLINE);


    if (recvfrom(sockfd, buffer, MAXLINE - 1, 0, NULL, NULL) < 0) {
        error("error");
    }

    printf("%s\n", buffer);
    close(sockfd);

}