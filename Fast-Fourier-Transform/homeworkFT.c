#include<stdio.h>
#include<math.h>
#include <stdlib.h>
#include <complex.h>

int P; // nr of threads
int N;
double PI;
FILE* input;
FILE* output;

double *inp;
double complex *result;

int min (int a, int b) {
	return a < b ? a : b;
}

void init() {

	inp = (double*) malloc(N * sizeof(double));
	result = (double complex*) malloc(N * sizeof(double complex));
	PI = atan2(1, 1) * 4;
}

void getArgs(int argc, char **argv) {
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

void* ft(void* var) {

	int tid   = *(int*)var;
    int start = tid * ceil((double)N / P);
	int end   = min(N, (tid + 1) * ceil((double)N / P));
	double complex sum;

	for (int k = start; k < end; ++k) {
		sum = 0.0;

		for (int n = 0; n < N; ++n) {
			sum += inp[n] * cexp(-2 * PI * I * k * n / N);
		}
		result[k] = sum;
	}
}

void free_the_mem() {

	free(inp);
	free(result);

}

int main(int argc, char **argv) {

	pthread_t tid[P];
    int thread_id[P];
	
	getArgs(argc, argv);
	fscanf(input, "%d,", &N);
	init();

	for (int i = 0; i < N; ++i) {
		fscanf(input, "%lf", &inp[i]);
	}
	
	for(int i = 0; i < P; i++)
		thread_id[i] = i;

	for(int i = 0; i < P; i++) {
		pthread_create(&(tid[i]), NULL, ft, &(thread_id[i]));
	}

	for(int i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}

	fprintf(output, "%d\n", N);
	for (int i = 0; i < N; ++i) {
		fprintf(output, "%lf %lf\n", creal(result[i]), cimag(result[i]));
	}

	free_the_mem();

   return 0;
}

