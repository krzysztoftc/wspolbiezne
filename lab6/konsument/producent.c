#include <stdio.h> 
#include <mqueue.h> 
#include "common.h"

int main(int argc, char *argv[]) { 
	unsigned int numer_producenta, kroki;
	sscanf(argv[1], "%d", &numer_producenta);
	sscanf(argv[2], "%d", &kroki );
	
	printf ("Producent numer %d, kroki %d\n", numer_producenta, kroki);
	
	bufor_t  *buf; 
	int  fd, res; // Deskryptor segmentu  
	
	if((fd=shm_open("Bufor",O_RDWR|O_CREAT,0664))==-1) { 
	    perror("shm_open"); 
	    exit(-1); 
	} 
	
	 // Okreslenie rozmiaru obszaru pamieci ---------------     
	res = ftruncate(fd,sizeof(bufor_t)); 
 	
 	if(res < 0) {
 		perror("ftrunc");
 		 return 0;
 	}
 	 
 	// Odwzorowanie segmentu fd w obszar pamieci procesow  
	 buf = (bufor_t *) mmap(0,sizeof(bufor_t), PROT_READ|PROT_WRITE,MAP_SHARED, fd, 0); 
	
	 if(buf == NULL) {
	 	perror("mmap"); 
	 	exit(-1); 
	 } 
	 
	int i;	
	for(i = 0; i < kroki; i++) { 	
		// Przeslanie komunikatu do kolejki 
		
		sem_wait(&(buf -> full));
		sem_wait(&(buf -> mutex));
		
		sprintf(buf->buf[buf->tail],"Producent %d krok %d",numer_producenta,i); 
		printf("Producent %d krok %d\n",numer_producenta,i); 
		
		buf -> cnt ++;
		buf -> head = (buf -> head + 1) % BSIZE;
		
		sem_post(&(buf -> mutex));
		sem_post(&(buf -> empty));
		
		 
		sleep(1); 
   } 

	return 0;
} 
