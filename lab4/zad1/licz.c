#include <stdlib.h>
#include <stdio.h>
#include <sys/file.h>
#include "wynik.h"

int pierwsza(int n){

	int i,j=0; 
  	for(i=2;i*i<=n;i++) { 
    	if(n%i == 0) return(0) ; 
 	} 

 return(1); 
}

//pocz koniec numer procesu
int main (int argc, char *argv[]){

	int pocz = (int)strtol(argv[1], NULL, 10);
	int kon = (int)strtol(argv[2], NULL, 10);
	int proc = (int)strtol(argv[3], NULL, 10);

	//printf("Uruchomiono licz z param: %d %d %d\n", pocz, kon, proc);

	int licz = 0,i;
	for (i = pocz ; i <= kon ; i++){
		if (pierwsza(i)) licz++;
	}
	//printf("Proces %d znalazl %d liczb pierwszych \n", proc, licz);
	
	int file;
	
	wynik_t wynik;
	wynik.pocz = pocz;
	wynik.kon = kon;
	wynik.ile = licz;
	
	file = open("wynik.bin", O_WRONLY | O_APPEND);
	if (file < 0) {
		perror("fopen"); 
		exit(1);
	}

	lockf(file, F_LOCK, 0);
	write(file, &wynik, sizeof(wynik));
	lockf(file, F_ULOCK, 0);
	close(file);

	exit(licz);
}
