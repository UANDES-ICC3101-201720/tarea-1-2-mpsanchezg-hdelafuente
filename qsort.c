#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIZE 24
#define swap(A, a, b) {unsigned tmp; tmp = A[a]; A[a] = A[b]; A[b] = tmp;}
#define MAXTHREADS 8
#define threshold SIZE/MAXTHREADS


static int A[SIZE];

static unsigned partition(int *A, unsigned low, unsigned high, unsigned pivot_index);
void read();
void *quicksort(void *arg);
static void quick_sort(int *A, unsigned low, unsigned high);
void print();

pthread_t pt[MAXTHREADS];

int main(int argc, char const *argv[])
{
	//double begin, end;
	int i, rc;
	void *exit_status;
	printf("CALLING THE READ FUNCTION\n");
	read();
	printf("CALLING THE PRINT FUNCTION\n");
	print();
	printf("CALLING THE SORT FUNCTION\n");

	fprintf(stderr, "hola\n"); //aqui se cae, borre el mutex y el cond porque no estaban haciendo nada
	//trata de coordinar la creacion de los threads con el mutex y el cond
	//te dejo el link del codigo original:
	/*
	http://multicoredays.blogspot.com/2014/05/parallel-quick-sort-using-pthreads.html

	para compilar usa:
	gcc -o qsort qsort.c -lm -lpthread -Wall
	*/
	if ((rc = pthread_create(&pt[i], NULL, *quicksort, (void *)&i))) 
	{
		fprintf(stderr, "%s\n", );
		printf("%d THREAD FAILED\n", i);
	}
	fprintf(stderr, "hola2\n");
	pthread_join(pt[i], &exit_status);
	printf("THREAD %d EXITED\n", i);
	print();
	return 0;
}


void* quicksort(void *arg) 
{

	int pivot_index, start, end, rc;
	int i;

	void *exit_status;
	printf("THREAD CREATED WITH I: %d\n", i);
	if (start >= end)
		return NULL;
	else 
	{
		pivot_index = (start+end)/2;
		pivot_index = partition(A, start, end, pivot_index);
		if ( ((pivot_index - start)<= threshold) || (i == MAXTHREADS - 1))
		{
			quick_sort(A, start, pivot_index);
		}
		else if (i < MAXTHREADS)
		{
			++i;
			if ((rc = pthread_create(&pt[i], NULL, *quicksort, (void*)&i)))
			{
				printf("%d THREAD FAILED\n", i);
			}
			pthread_join(pt[i], &exit_status);
			printf("%d THREAD EXITED\n", i);
		}

		if ( ((end - pivot_index) <= threshold) || (i == MAXTHREADS - 1))
		{
			quick_sort(A, pivot_index, end);
		}

		else if (i < MAXTHREADS)
		{
			++i;
			if ((rc = pthread_create(&pt[i], NULL, *quicksort, (void*)&i)))
			{
				printf("%d THREAD FAILED\n", i);
			}

			pthread_join(pt[i], &exit_status);
			printf("%d THREAD EXITED\n", i);
		}
	return NULL;
	}
}
static unsigned partition(int *A, unsigned low, unsigned high, unsigned pivot_index) {
	if(pivot_index != low) swap(A, low, pivot_index);

	pivot_index = low;
	low++;
	while (low <= high) {
		
		if (A[low] <= A[pivot_index]) 
			low++;

		else if (A[high] > A[pivot_index]) 
			high--;

		else 
			swap(A, low, high);
	}

	if (high != pivot_index)
	{
		swap(A, pivot_index, high);
	}
	return high;

}

static void quick_sort(int *A, unsigned low, unsigned high) {
	unsigned pivot_index;
	if (low >= high)
	{
		return;
	}

	else {
		pivot_index = (low + high)/2;
		pivot_index = partition(A, low, high, pivot_index);
		if (low < pivot_index)
		{
			quick_sort(A, low, pivot_index-1);
		}
		if (pivot_index < high)
		{
			quick_sort(A, pivot_index+1, high);
		}
	}
}

void read() {
	for (int i = 0; i < SIZE; ++i)
	{
		A[i] = rand()%10 + 1;
	}
}

void print() {
	int chunk;
	chunk = SIZE / MAXTHREADS;
	for (int i = 0; i < SIZE; ++i)
	{
		if (i%chunk == 0)
		{
			printf("|");
		}
		printf(" %d ", A[i]);
	}
	printf("|\n\n");
}