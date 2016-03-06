#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <pthread.h>

int **matrix = NULL;
int **tmatrix = NULL;

int** create_matrix(int n, int m) {
	int **temp_matrix = NULL;
	int i, j;
	temp_matrix = (int **)malloc(sizeof(int *)*n);
	srand((unsigned int) time(NULL));
	for(i = 0; i < n; i++)
		temp_matrix[i] = malloc(sizeof(int)*m);

	return temp_matrix;
}

int** generate_random_matrix(int **matrix, int n, int m) {
	int **temp_matrix = matrix;
	int i, j;
	for(i = 0; i < n; i++)
		for(j = 0; j < m; j++)
			temp_matrix[i][j] = rand() % 1000;
	return temp_matrix;
}

void show_matrix(int **matrix, int n, int m) {
	int i, j;
	for(i = 0; i < n; i++) {
		for(j = 0; j < m; j++)
			printf("%d ", matrix[i][j]);
		printf("\n");	
	}
	printf("\n\n");	
}

void calculate_sequential_transpose(int n, int m) {
	tmatrix = create_matrix(m, n);
	int i, j;
	for(i = 0; i < n; i++)
		for(j = 0; j < m; j++)
			tmatrix[j][i] = matrix[i][j];
}

// void calculate_parallel_transpose(int n, int m) {
// 	int num_workers = get_nprocs();
// 	if(num_workers > n)
// 		num_workers = n;


// }


int main(int argc, char* argv[]) {

	int n, m, i, j;
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <n> <m>\n", argv[0]);
		exit(0);
	}
	
	n = atoi(argv[1]);
	m = atoi(argv[2]);
	matrix = create_matrix(n, m);
	matrix = generate_random_matrix(matrix, n, m);
	show_matrix(matrix, n, m);
	calculate_sequential_transpose(n, m);
	calculate_parallel_transpose(n, m);

	show_matrix(tmatrix, m, n);

	for(j = 0; j < m; j++)
		free(tmatrix[j]);

	free(tmatrix);


	for(i = 0; i < n; i++)
		free(matrix[i]);

	free(matrix);

	return 0;
}



