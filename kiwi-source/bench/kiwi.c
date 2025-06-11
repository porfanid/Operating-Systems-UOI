#include "../engine/db.h"
#include "../engine/variant.h"
#include "bench.h"

pthread_mutex_t totalPut;		//Orismos kleidarias amoibaiou apokleismou(mutex) gia tis sunolikes eggrafes
pthread_mutex_t totalGet;		//Orismos kleidarias amoibaiou apokleismou(mutex) gia tis sunolikes anagnwseis
double costOfTotalPut;			//Orismos sunolikou kostous (xronou) eggrafwn
double costOfTotalGet;			//Orismos sunolikou kostous (xronou) anagnwsewn

#define DATAS ("testdb")

DB* db;				//Orismos ths bashs ws global metablhth kai oxi entos twn sunarthsewn _write_test() kai _read_test()

//Anoigma bashs
void _open_db()
{	
	db = db_open(DATAS);		//Klhsh ths sunarthshs db_open() tou db.c
}

//Kleisimo bashs
void _close_db()
{	db_close(db);			//Klhsh ths sunarthshs db_close() tou db.c
}

void _write_test(long int count, int r, int threads)
{
	int i;
	long int newCount = count/threads; 		//Neo plh8os eisagomenwn stoixeiwn kai diamoirasmos twn eggrafwn sta nhmata
	struct timeval start,end;	//Apo long long ta orisame ws struct timeval
	double cost;
	//long long start,end;
	Variant sk,sv;
        char key[KSIZE + 1];
	char val[VSIZE + 1];
	char sbuf[1024];
        memset(key, 0, KSIZE + 1);
	memset(val, 0, VSIZE + 1);
	memset(sbuf, 0, 1024);
	//start = get_ustime_sec();
	gettimeofday(&start, NULL);	//Pairnoume ton arxiko xrono
	for (i=0; i<newCount; i++) {
		if (r)
			_random_key(key, KSIZE);
		else
			snprintf(key, KSIZE, "key-%d", i);
		fprintf(stderr, "%d adding %s\n", i, key);
		snprintf(val, VSIZE, "val-%d", i);
                
		sk.length = KSIZE;
		sk.mem = key;
		sv.length = VSIZE;
		sv.mem = val;
                
		db_add(db, &sk, &sv);
		if ((i % 10000) == 0) {
			fprintf(stderr,"random write finished %d ops%30s\r", 
					i, 
					"");
                        fflush(stderr);
		}
	}
        //end = get_ustime_sec();
	gettimeofday(&end, NULL);	//Pairnoume ton teliko xrono
	//cost = end - start;
	cost = (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;		//Ypologismos kostous(xronou) ths trexousas eggrafhs
	pthread_mutex_lock(&totalPut);				//Kleidwma me mutex twn sunolikwn eggrafwn prin mpoume sthn krisimh perioxh
	//Arxh krisimhs perioxhs
	costOfTotalPut += cost;					//Pros8etoume sto sunoliko kostos(xrono) twn nhmatwn eggraghs to kostos ths trexousas eggrafhs
	//Telos krisimhs perioxhs 
	pthread_mutex_unlock(&totalPut);			//3ekleidwma me mutex twn sunolikwn eggrafwn,afou ektelestei h krisimh perioxh
}

void _read_test(long int count, int r, int threads)
{
	int i;
	long int newCount = count/threads;		//Neo plh8os anazhtoumenwn stoixeiwn kai diamoirasmos twn anagnwsewn sta nhmata
	struct timeval start,end;	//Apo long long ta orisame ws struct timeval
	int ret;
	int found = 0;
	double cost;
	//long long start,end;
	Variant sk;
	Variant sv;
	char key[KSIZE + 1];
	//start = get_ustime_sec();
	gettimeofday(&start, NULL);	//Pairnoume ton arxiko xrono
	for (i = 0; i < newCount; i++) {
		memset(key, 0, KSIZE + 1);

                /* if you want to test random write, use the following */
		if (r)
			_random_key(key, KSIZE);
		else
			snprintf(key, KSIZE, "key-%d", i);
		fprintf(stderr, "%d searching %s\n", i, key);
		sk.length = KSIZE;
		sk.mem = key;
		//sleep(0.10);
		ret = db_get(db, &sk, &sv);
		if (ret) {
			//db_free_data(sv.mem);
			found++;
		} else {
			INFO("not found key#%s", 
					sk.mem);
    		}
                
		if ((i % 10000) == 0) {
			fprintf(stderr,"random read finished %d ops%30s\r", 
					i, 
					"");
                        fflush(stderr);
		}
	}
        //end = get_ustime_sec();
	gettimeofday(&end, NULL);				//Pairnoume ton teliko xrono
	//cost = end - start;
	cost = (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;		//Ypologismos kostous(xronou) ths trexousas anagnwshs
        pthread_mutex_lock(&totalGet);				//Kleidwma me mutex twn sunolikwn anagnwsewn prin mpoume sthn krisimh perioxh
	//Arxh krisimhs perioxhs
	costOfTotalGet += cost;		//Pros8etoume sto sunoliko kostos(xrono) twn nhmatwn anagnwshs to kostos ths trexousas anagnwshs
	//Telos krisimhs perioxhs
	pthread_mutex_unlock(&totalGet);			//3ekleidwma me mutex twn sunolikwn anagnwsewn,afou ektelestei h krisimh perioxh
}
