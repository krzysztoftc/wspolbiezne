#include <stdio.h> 
#include <mqueue.h> 
#include "common.h"

int main(int argc, char *argv[]) { 
	unsigned int numer_producenta, kroki;
	sscanf(argv[1], "%d", &numer_producenta);
	sscanf(argv[2], "%d", &kroki );
	
	printf ("Producent numer %d, kroki %d\n", numer_producenta, kroki);
	msg_type msg ;
	int i, res; 
	unsigned int prior; 
	mqd_t  mq; 
	struct mq_attr attr; 
	
	/* Utworzenie kolejki komunikatow -----------------*/ 
	attr.mq_msgsize = sizeof(msg); 
	attr.mq_maxmsg = 8; 
	attr.mq_flags = 0; 
	int priority = 1;
	mq=mq_open("/Kolejka", O_RDWR, 0660, NULL ); 
	printf("sizeof msg: %ld\n", sizeof(msg));
	for(i = 0; i < kroki; i++) { 
		msg.pnr = numer_producenta; 
		msg.type = 1; 
		sprintf(&msg.text,"Producent %d krok %d",numer_producenta,i); 
		printf ("Sle komunikat: %s\n", msg.text);
		// Przeslanie komunikatu do kolejki 
		res = mq_send(mq,&msg,sizeof(msg),priority); 
		 
		sleep(1); 
   } 
   	mq_close(mq);

	return 0;
} 
