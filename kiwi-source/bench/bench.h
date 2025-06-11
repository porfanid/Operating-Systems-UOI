#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define KSIZE (16)
#define VSIZE (1000)

extern pthread_mutex_t totalPut;	//Kleidaria amoibaiou apokleismou(mutex) gia tis sunolikes eggrafes
extern pthread_mutex_t totalGet;	//Kleidaria amoibaiou apokleismou(mutex) gia tis sunolikes anagnwseis
extern double costOfTotalPut;		//Sunoliko kostos(xronos) eggrafwn
extern double costOfTotalGet;		//Sunoliko kostos(xronos) anagnwsewn

extern int threads;			//Plh8os twn nhmatwn

#define LINE "+-----------------------------+----------------+------------------------------+-------------------+\n"
#define LINE1 "---------------------------------------------------------------------------------------------------\n"

long long get_ustime_sec(void);
void _random_key(char *key,int length);

void _write_test(long int count, int r, int threads);		//Prwtotupo sunarthshs _write_test() tou arxeiou kiwi.c
void _read_test(long int count, int r, int threads);			//Prwtotupo sunarthshs _read_test() tou arxeiou kiwi.c

void _open_db();							//Prwtotupo sunarthshs _open_db() tou arxeiou kiwi.c
void _close_db();							//Prwtotupo sunarthshs _close_db() tou arxeiou kiwi.c

//Orisame auth th domh, wste na apoktoume prosbash se authn mesw twn orismatwn args,args1 kai args2 tou bench.c 
struct data
{   
	long int myCount;		//Plh8os eisagomenwn/anazhtoumenwn stoixeiwn
	int my_r;			//Isoutai eite me 0,eite me 1
    	int myThreads;			//Plh8os nhmatwn
};
