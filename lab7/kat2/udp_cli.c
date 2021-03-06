// Proces wysyla a potem odbiera komunikaty udp
// Wspolpracuje z udp_serw
// Kompilacja gcc udp_cli.c -o udp_cli -lrt
 #include <netinet/in.h>
 #include <stdio.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <unistd.h>
 #include <string.h>
 #include "common.h"
 #define BUFLEN 80
 #define KROKI 10
 #define PORT 9950
 #define SRV_IP "127.0.0.1"

typedef struct {
  int typ;
  char buf[BUFLEN];
} msgt;

  void blad(char *s) {
	  perror(s);
	  _exit(1);
	}

int main(int argc, char * argv[]) {
	struct sockaddr_in adr_moj, adr_serw, adr_x;
	int s, i, slen=sizeof(adr_serw), snd, blen=sizeof(msgt),rec;
	char buf[BUFLEN];
	mms_t msg;

	s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(s < 0) blad("socket");
	printf("Gniazdko %d utworzone\n",s);
	memset((char *) &adr_serw, 0, sizeof(adr_serw));
	adr_serw.sin_family = AF_INET;
	adr_serw.sin_port = htons(PORT);
	if (inet_aton(argv[1], &adr_serw.sin_addr)==0) {
		  fprintf(stderr, "inet_aton() failed\n");
		  _exit(1);
	}

	for (;;) {
		msg.typ = 1;
		
		char message[SIZE] = {'\0'};
		printf("? ");
		scanf("%s", message);
		message[SIZE - 1] = '\0';
		
		if (!strcmp(message, "write")){
			msg.typ = WRITE;
			
			printf("Podaj numer deskryptora: ");
			scanf("%d", &msg.fh);
			
			printf("Podaj tresc: ");
			scanf("%s", msg.buf);

			msg.ile = strlen(msg.buf);

		}
		
		else if (!strcmp(message, "read") ){
			msg.typ = READ;
	
			printf("Podaj numer deskryptora: ");
			scanf("%d", &msg.fh);

			printf("Podaj ilosc bajtow: ");
			scanf("%d", &msg.ile);
			
			memset(msg.buf, '\0', SIZE);
		}
		
		else if (!strcmp(message, "openr")){
			msg.typ = OPENR;

			printf("Podaj nazwe pliku: ");
			scanf("%s", msg.buf);

			msg.ile = strlen(msg.buf);
			msg.fh = -1;
		}
		
		else if (!strcmp(message, "openw")){
			msg.typ = OPENW;
			
			printf("Podaj nazwe pliku: ");
			scanf("%s", msg.buf);

			msg.ile = strlen(msg.buf);
			msg.fh = -1;
		}
		
		else if (!strcmp(message, "close")){
			msg.typ = CLOSE;
			
			printf("Podaj numer deskryptora: ");
			scanf("%d", &msg.fh);
		}
		
		else if (!strcmp(message, "stop")){
			msg.typ = STOP;
		}
		
		else if (!strcmp(message, "get")){
			int get = 0;
			
			printf("Podaj nazwe pliku: ");
			scanf("%s", msg.buf);

			msg.ile = strlen(msg.buf);
			msg.fh = -1;
			msg.typ = OPENR;
			
			snd = sendto(s, &msg, blen, 0,(struct sockaddr *) &adr_serw,(socklen_t) slen);
			if(snd < 0) blad("sendto()");
			
			rec = recvfrom(s, &msg, blen, 0,(struct sockaddr *) &adr_x,(socklen_t *) &slen);
			if(rec < 0) blad("recvfrom()");
			
			int fh = msg.fh;
			printf	("\n");
			
			do{
			msg.typ = READ;
			msg.ile = SIZE;
			msg.fh = fh;
			
			snd = sendto(s, &msg, blen, 0,(struct sockaddr *) &adr_serw,(socklen_t) slen);
			if(snd < 0) blad("sendto()");
			
			rec = recvfrom(s, &msg, blen, 0,(struct sockaddr *) &adr_x,(socklen_t *) &slen);
			printf("%s", msg.buf);
			get = msg.ile;
			
			}while(get == SIZE);
			
			msg.typ = CLOSE;
			msg.fh = fh;
			
			snd = sendto(s, &msg, blen, 0,(struct sockaddr *) &adr_serw,(socklen_t) slen);
			if(snd < 0) blad("sendto()");
			rec = recvfrom(s, &msg, blen, 0,(struct sockaddr *) &adr_x,(socklen_t *) &slen);
		}
		
		else{
			printf("Niepoprawne polecenie, probuj dalej\n");
			continue;
		}
		
		snd = sendto(s, &msg, blen, 0,(struct sockaddr *) &adr_serw,(socklen_t) slen);
		if(snd < 0) blad("sendto()");
		printf("Wyslano komunikat res: %d\n", snd);
		printf("Czekam na odpowiedz\n");
		rec = recvfrom(s, &msg, blen, 0,(struct sockaddr *) &adr_x,(socklen_t *) &slen);
		if(rec < 0) blad("recvfrom()");
		printf("Otrzymana odpowiedz %s fh %d \n",msg.buf, msg.fh);
		sleep(1);
	}
	close(s);
	return 0;
}


