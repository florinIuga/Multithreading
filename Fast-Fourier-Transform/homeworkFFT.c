#include <stdio.h>
#include <complex.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double PI;
typedef double complex cplx;

int P; // nr of threads
int N;
FILE* input;
FILE* output;
pthread_barrier_t barrier;
pthread_mutex_t lock;

double complex *buf;
double complex *out;

int min (int a, int b) {
	return a < b ? a : b;
}

void getArgs(int argc, char **argv)
{
	if(argc < 4) {
		printf("Not enough paramters: ./program in out nr_threads\n");
		exit(1);
	}
	input = fopen(argv[1], "r");
	if (input == NULL) {
		perror("fopen");
	}
	output = fopen(argv[2], "w");

	if (output == NULL) {
		perror("fopen");
	}

	P = atoi(argv[3]);
}
 
void init() {

	buf = (double complex*) malloc(N * sizeof(double complex));
	out = (double complex*) malloc(N * sizeof(double complex));
	PI = atan2(1, 1) * 4;
	pthread_barrier_init(&barrier, NULL, P);
}

void _fft(cplx buf[], cplx out[], int step)
{
	if (step < N) {
		
		_fft(out, buf, step * 2);
		_fft(out + step, buf + step, step * 2);

 		for (int i = 0; i < N; i += 2 * step) {
			cplx t = cexp(-I * PI * i / N) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + N)/2] = out[i] - t;
		}
	}
}
 
void* fft(void *ID)
{
	int tid = *(int*)ID;
	int start = tid * ((double)N / P);
	int end = min(N, (tid + 1) * ((double)N / P));
	int step;  

	for (int i = 0; i < N; i++) 
		out[i] = buf[i];
 
	// 1,2 or 4 threads
		/* nr_threads = 1 => plec cu recursivitatea
		de pe nivelul 0, deci este fft simplu */
		if (P == 1) {
			 step = 1;
			_fft(buf, out, step);
			
		/* nr_threads = 2 => plec cu recursivitatea
		de pe nivelul 1, deci step-ul va fi 1, un thread
		va merge pe subarborele stang, iar celalalt pe cel
		drept si va trebui calculat manual elementul de pe nivelul 0 (root-ul)*/
		} else if (P == 2) {
			 step = 1;

				_fft(out + tid, buf + tid, step * 2);

				for (int i = 0; i < N; i += 2*step) {
					cplx t = cexp(-I * PI * i / N) * out[i + step];
					buf[i / 2]     = out[i] + t;
					buf[(i + N)/2] = out[i] - t;
				}
		
		/* nr_threads = 4 => plec cu recursivitatea
		de pe nivelul 2, deci step-ul va fi 2*/
		} else if (P == 4) {
			 step = 2;

			_fft(buf + tid, out + tid, step * 2);
			pthread_barrier_wait(&barrier);

			/* calculeaza elementele de pe nivelul 1 */
			
			for (int i = start; i < end; i += 2*step) {
				cplx t = cexp(-I * PI * i / N) * buf[i + step];
				out[i / 2]     = buf[i] + t;
				out[(i + N)/2] = buf[i] - t;
			}
			
			
			buf = (buf + 1);
			out = (out + 1);
			
			for (int i = start; i < end; i += 2*step) {
				cplx t = cexp(-I * PI * i / N) * buf[i + step];
				out[i / 2]     = buf[i] + t;
				out[(i + N) / 2] = buf[i] - t;
			}

			/* calculeaza si radacina */
			step = 1;
			buf = (buf - 1);
			out = (out - 1);

			for (int i = start; i < end; i += 2*step) {
				cplx t = cexp(-I * PI * i / N) * out[i + step];
				buf[i / 2]     = out[i] + t;
				buf[(i + N)/2] = out[i] - t;
			}
		}
}

void free_the_mem() {

	free(buf);
	free(out);
	pthread_barrier_destroy(&barrier);
}

int main(int argc, char **argv)
{

	pthread_t tid[P];
    int thread_id[P];

	getArgs(argc, argv);
	fscanf(input, "%d,", &N);
	init();

	for (int i = 0; i < N; ++i) {
		fscanf(input, "%lf", &buf[i]);
	}
	
	for(int i = 0; i < P; i++)
		thread_id[i] = i;

	for(int i = 0; i < P; i++) {
		pthread_create(&(tid[i]), NULL, fft, &(thread_id[i]));
	}

	for(int i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}
	
	fprintf(output, "%d\n", N);
	for (int i = 0; i < N; ++i) {
		fprintf(output, "%lf %lf\n", creal(buf[i]), cimag(buf[i]));
	}

 	free_the_mem();

	return 0;
}

