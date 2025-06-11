#include "bench.h"


void _random_key(char *key,int length) {
	int i;
	char salt[36]= "abcdefghijklmnopqrstuvwxyz0123456789";

	for (i = 0; i < length; i++)
		key[i] = salt[rand() % 36];
}

void _print_header(int count)
{
	double index_size = (double)((double)(KSIZE + 8 + 1) * count) / 1048576.0;
	double data_size = (double)((double)(VSIZE + 4) * count) / 1048576.0;

	printf("Keys:\t\t%d bytes each\n", 
			KSIZE);
	printf("Values: \t%d bytes each\n", 
			VSIZE);
	printf("Entries:\t%d\n", 
			count);
	printf("IndexSize:\t%.1f MB (estimated)\n",
			index_size);
	printf("DataSize:\t%.1f MB (estimated)\n",
			data_size);

	printf(LINE1);
}

void _print_environment()
{
	time_t now = time(NULL);

	printf("Date:\t\t%s", 
			(char*)ctime(&now));

	int num_cpus = 0;
	char cpu_type[256] = {0};
	char cache_size[256] = {0};

	FILE* cpuinfo = fopen("/proc/cpuinfo", "r");
	if (cpuinfo) {
		char line[1024] = {0};
		while (fgets(line, sizeof(line), cpuinfo) != NULL) {
			const char* sep = strchr(line, ':');
			if (sep == NULL || strlen(sep) < 10)
				continue;

			char key[1024] = {0};
			char val[1024] = {0};
			strncpy(key, line, sep-1-line);
			strncpy(val, sep+1, strlen(sep)-1);
			if (strcmp("model name", key) == 0) {
				num_cpus++;
				strcpy(cpu_type, val);
			}
			else if (strcmp("cache size", key) == 0)
				strncpy(cache_size, val + 1, strlen(val) - 1);	
		}

		fclose(cpuinfo);
		printf("CPU:\t\t%d * %s", 
				num_cpus, 
				cpu_type);

		printf("CPUCache:\t%s\n", 
				cache_size);
	}
}

//Kanoume aithsh gia eggrafh
void *write_request(void *arg)
{	struct data *d = (struct data *)arg;
	_write_test(d->myCount, d->my_r, d->myThreads);			//Klhsh ths _write_test() tou kiwi.c,wste na ginei h eggrafh pou zhtame
	return 0;
}

//Kanoume aithsh gia anagnwsh
void *read_request(void *arg)
{	struct data *d = (struct data *)arg;
	_read_test(d->myCount, d->my_r, d->myThreads);			//Klhsh ths _read_test() tou kiwi.c,wste na ginei h anagnwshh pou zhtame
	return 0;
}

int main(int argc,char** argv)
{
	int r = 0;
	if (argc < 4) {
		fprintf(stderr,"Usage: db-bench <write | read | readwrite> <count> <threads> <percentage>\n");		//Pros8esame thn periptwsh readwrite, to plh8os twn nhmatwn (threads) kai to pososto eggrafhs (percentage) gia thn periptwsh readwrite
		exit(1);
	}
	int threads = atoi(argv[3]);		//Plh8os nhmatwn
	long int count;
	int i;
	pthread_t threadsGetID[threads], threadsPutID[threads];	//Pinakes tautothtwn(anagnwristikwn) nhmatwn pou exoun megethos iso me to plhthos twn nhmatwn
	struct data argsPut,argsGet;			//Orismata twn sunarthsewn pthread_create()
	pthread_mutex_init(&totalPut,NULL);		//Arxikopoihsh kleidarias amoibaiou apokleismou(mutex) gia tis sunolikes eggrafes
    	pthread_mutex_init(&totalGet,NULL);		//Arxikopoihsh kleidarias amoibaiou apokleismou(mutex) gia tis sunolikes anagnwseis
	costOfTotalPut = 0;				//Arxikopoihsh sunolikou kostous(xronou) eggrafwn sto 0
	costOfTotalGet = 0;				//Arxikopoihsh sunolikou kostous(xronou) anagnwsewn sto 0
	srand(time(NULL));
	
	if (strcmp(argv[1], "write") == 0) {
		
		count = atoi(argv[2]);
		_print_header(count);
		_print_environment();
		if (argc == 4)
			r = 1;
		_open_db();				//Anoigma bashs
		argsPut.myCount = count;			//Plh8os eisagomenwn stoixeiwn
		argsPut.my_r = r;				//Isoutai eite me 0,eite me 1
		argsPut.myThreads = threads;		//Plh8os nhmatwn
		for (i=0; i<threads; i++)
			pthread_create(&threadsPutID[i],NULL,write_request,(void *) &argsPut);			//Dhmiourgia nhmatwn
		for (i=0; i<threads; i++)
			pthread_join(threadsPutID[i],NULL);							//Anamonh termatismou nhmatwn
		_close_db();				//Kleisimo bashs
		double average_time = (double)(costOfTotalPut / argsPut.myCount);						//Ypologismos tou mesou kostous(xronou) eggrafwn 
		double average_time_sec = (double)average_time/(double)1000000;							//Metatroph tou mesou kostous(xronou) eggrafwn se deuterolepta
		double throughput = (double)(argsPut.myCount / costOfTotalPut);						//Ypologismos ths rythmapodoshs eggrafwn
		double throughput_sec = (double)throughput/(double)1000000;						//Metatroph ths rythmapodoshs eggrafwn se deuterolepta
		printf(LINE);
		printf("|Random-Write	(Requests:%ld): %.30f sec/write %.30f writes/sec(estimated) cost:%.30f sec\n"
		,argsPut.myCount, average_time_sec
		,throughput_sec
		,(double)costOfTotalPut/(double)1000000);
	} else if (strcmp(argv[1], "read") == 0) {
		count = atoi(argv[2]);
		_print_header(count);
		_print_environment();
		if (argc == 4)
			r = 1;
		_open_db();				//Anoigma bashs
		argsGet.myCount = count;			//Plh8os anazhtoumenwn stoixeiwn
		argsGet.my_r = r;				//Isoutai eite me 0,eite me 1
		argsGet.myThreads = threads;		//Plh8os nhmatwn
		for (i=0; i<threads; i++)
			pthread_create(&threadsGetID[i],NULL,read_request,(void *) &argsGet);			//Dhmiourgia nhmatwn
		for (i=0; i<threads; i++)
			pthread_join(threadsGetID[i],NULL);							//Anamonh termatismou nhmatwn
		_close_db();				//Kleisimo bashs
		double average_time = (double)(costOfTotalGet / argsGet.myCount);							//Ypologismos tou mesou kostous(xronou) anagnwsewn 
		double average_time_sec = (double)average_time/(double)1000000;							//Metatroph tou mesou kostous(xronou) anagnwsewn se deuterolepta
		double throughput = (double)(argsGet.myCount / costOfTotalGet);						//Ypologismos ths rythmapodoshs anagnwsewn
		double throughput_sec = (double)throughput/(double)1000000;						//Metatroph ths rythmapodoshs anagnwsewn se deuterolepta
		printf(LINE);
		printf("|Random-Read	(Requests:%ld): %.30f sec/read %.30f reads/sec(estimated) cost:%.30f sec\n",
		argsGet.myCount, average_time_sec,
		throughput_sec,
		(double)costOfTotalGet/(double)1000000);
	} else if (strcmp(argv[1], "readwrite") == 0) {
		count = atoi(argv[2]);
		int percentage = atoi(argv[4]);				// Pososto eggrafwn. To pososto twn anagnwsewn prokyptei apo thn praxh 100-percentage
		if(percentage>100||percentage<0){
			fprintf(stderr,"The percentage must be a number between 0-100\n");
			exit(1);
		}
		_print_header(count);
		_print_environment();
		if (argc == 5)
			r = 1;
		_open_db();					//Anoigma bashs
		argsPut.myCount = (long)(count * percentage/100);		//Plh8os eisagomenwn stoixeiwn
		argsPut.my_r = r;					//Isoutai eite me 0,eite me 1
		argsPut.myThreads = (int)(threads * percentage/100);	//Plh8os nhmatwn eggrafhs
		argsGet.myCount = (long)(count * (100-percentage)/100);		//Plh8os anazhtoumenwn stoixeiwn
		argsGet.my_r = r;					//Isoutai eite me 0,eite me 1
		argsGet.myThreads = (int)(threads * (100-percentage)/100);	//Plh8os nhmatwn anagnwshs
		for (i=0; i<(threads * percentage/100); i++)
			pthread_create(&threadsPutID[i],NULL,write_request,(void *) &argsPut);		//Dhmiourgia nhmatwn eggrafhs
		for (i=0; i<(threads * (100-percentage)/100); i++)
			pthread_create(&threadsGetID[i],NULL,read_request,(void *) &argsGet);		//Dhmiourgia nhmatwn anagnwshs
		for (i=0; i<(threads * (100-percentage)/100); i++)
			pthread_join(threadsGetID[i],NULL);						//Anamonh termatismou nhmatwn anagnwshs
		for (i=0; i<(threads * percentage/100); i++)
			pthread_join(threadsPutID[i],NULL);						//Anamonh termatismou nhmatwn eggrafhs
		_close_db();					//Kleisimo bashs
		double average_time_put = (double)(costOfTotalPut / argsPut.myCount);				//Ypologismos tou mesou kostous(xronou) eggrafwn 
		double average_time_put_sec = (double)average_time_put/(double)1000000;				//Metatroph tou mesou kostous(xronou) eggrafwn se deuterolepta
		double throughput_put = (double)(argsPut.myCount / costOfTotalPut);					//Ypologismos ths rythmapodoshs eggrafwn
		double throughput_put_sec = (double)throughput_put/(double)1000000;					//Metatroph ths rythmapodoshs eggrafwn se deuterolepta
		double average_time_get = (double)(costOfTotalGet / argsGet.myCount);				//Ypologismos tou mesou kostous(xronou) anagnwsewn 
		double average_time_get_sec = (double)average_time_get/(double)1000000;				//Metatroph tou mesou kostous(xronou) anagnwsewn se deuterolepta
		double throughput_get = (double)(argsGet.myCount / costOfTotalGet);					//Ypologismos ths rythmapodoshs anagnwsewn
		double throughput_get_sec = (double)throughput_get/(double)1000000;					//Metatroph ths rythmapodoshs anagnwsewn se deuterolepta
		printf(LINE);
		printf("|Random-Write	(Requests:%ld): %.30f sec/write %.30f writes/sec(estimated) cost:%.30f sec\n"
		,argsPut.myCount, average_time_put_sec
		,throughput_put_sec
		,(double)costOfTotalPut/(double)1000000);
		printf(LINE);
		printf("|Random-Read	(Requests:%ld): %.30f sec/read %.30f reads/sec(estimated) cost:%.30f sec\n",
		argsGet.myCount, average_time_get_sec,
		throughput_get_sec,
		(double)costOfTotalGet/(double)1000000);
	} else {
		fprintf(stderr,"Usage: db-bench <write | read | readwrite> <count> <threads> <percentage> \n");		//Pros8esame thn periptwsh readwrite, to plh8os twn nhmatwn (threads) kai to pososto eggrafhs (percentage) gia thn periptwsh readwrite
		exit(1);
	}

	return 1;
}


