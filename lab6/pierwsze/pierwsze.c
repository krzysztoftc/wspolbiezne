//execl(”./licz”,”licz”,pocz,kon,numerP,0 )
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h> 

#include <time.h>
#include "common.h"

int pierwsza(int n){

	if (n < 2) return 0;
	int i,j=0; 
  	for(i=2;i*i<=n;i++) { 
    	if(n%i == 0) return(0) ; 
 	} 

 return(1); 
}


//pocz koniec procesy
int main (int argc, char *argv[]){

	double start = time(NULL);

	int pocz = (int)strtol(argv[1], NULL, 10);
	int kon = (int)strtol(argv[2], NULL, 10);
	int proc = (int)strtol(argv[3], NULL, 10);
	int krok = (kon - pocz) / proc;
	
	bufor_t  *buf; 
	int  fd, res; // Deskryptor segmentu  
	
	shm_unlink("Bufor1");    
	// Utworzenie segmentu pamieci  ---------------------  
	
	 if((fd=shm_open("Bufor1",O_RDWR|O_CREAT,0664))==-1) { 
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
	
	//inicjalizacja obszaru
	buf -> head = 0;
	buf -> tail = 0;
	buf -> cnt = 0;
	
	if(sem_init(&(buf->mutex), 1, 1)){ 
		perror("mutex");
		exit(0);  
	}
	
	if(sem_init(&(buf->empty), 1, 0)){ 
		perror("mutex");
		exit(0);  
	}
	
	if(sem_init(&(buf->full), 1, 8)){ 
		perror("mutex");
		exit(0);  
	}
	
	printf ("Init OK\n");

	printf ("Zakres: %d %d procesow: %d\n", pocz, kon, proc); 
	
	int p, i;
	for (i = 0, p = pocz; i < proc ; i++, p += krok +1) {
		pid_t pid;
		printf("fork %d\n", i);
		if(fork() == 0) {
			//int e = execl("./licz", "licz", (char*)p, (char*)(p+krok), (char*)(i),(char*)NULL);
	
			
			int k = (i == proc -1) ? kon:  p+krok;

			int j, sum = 0;
			
			for (j = p; j <=  k ; j++){
				if (pierwsza (j)) sum++;
			}
			
			printf("Proces %d znalazl %d liczb pierwszych\n",i,sum);
			
			sem_wait(&(buf -> full));
			sem_wait(&(buf -> mutex));
		 
			 
			buf->buf[buf->head].pocz = j;
			buf->buf[buf->head].kon = (j == proc -1) ? kon: p+krok;
			buf->buf[buf->head].ile = sum;
		
			buf -> cnt ++;
			buf -> head = (buf -> head + 1) % BSIZE;
		
			sem_post(&(buf -> mutex));
			sem_post(&(buf -> empty));
			
			exit(0);
			}
			
			
	}
	
	printf ("Forki ok \n");

	int suma = 0;
	
	wynik_t readed;
	
	for (i = 0; i < proc; i++){
	
		sem_wait(&(buf -> empty));
		sem_wait(&(buf -> mutex));
		
		suma += buf->buf[buf->tail].ile; 
		printf ("Odebrano komunikat [%d] : %d \n", i, suma);
		buf -> cnt --;
		buf -> tail = (buf -> tail + 1) % BSIZE;
		
		sem_post(&(buf -> mutex));
		sem_post(&(buf -> full));
		

		//printf("Zakonczono %d exit: %d\n", pid, WEXITSTATUS(status));
	}
	
	printf("Odczytano wszystko\n");

	for (i = 0; i < proc; i++){
		int status;
		pid_t pid = wait(&status);
//		printf("Zakonczono %d exit: %d\n", pid, WEXITSTATUS(status));
	}


	double koniec = time(NULL);
	printf("\nLiczb pierwszych w zakresie [%d,%d] jest: %d obliczenie zajely: %f [s]\n\n", pocz, kon, suma, (koniec - start));
}
