#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
//#include "types.h"
//#include "const.h"
//#include "util.h"
#include <ctype.h>
#include <pthread.h>
//#include <limits.h>
#include <signal.h>

int MAXTHREADS;
int n;
int current_thread = 0;

int *A;


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


void swap(int *A, int a, int b) {
    int tmp;
    tmp = A[a]; 
    A[a] = A[b]; 
    A[b] = tmp;
}

int partition (int* A, int lo, int hi){
    int pivot = A[hi]; // pivot
    int i = (lo - 1);     // Index of smaller element
    
    for (int j = lo; j <= hi-1; j++){
        
        // If current element is smaller than or
        // equal to pivot
        if (A[j] <= pivot){
            i++;
            swap(A, i, j);
        }
    }
    swap(A, i + 1, hi);
    return (i + 1);
}

void quicksort (int* arr, int left, int right) {
    int i = left, j = right;
    int tmp;
    int pivot = arr[(left + right) / 2];

    while (i <= j) {
        while (arr[i] < pivot)
            i++;
        while (arr[j] > pivot)
            j--;
        if (i <= j) {
            tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
            j--;
        }
    }
    if (left < j) { 
        quicksort(arr, left, j);  
    }
    if (i < right) { 
        quicksort(arr, i, right); 
    }
}

typedef struct{
	int n;
	int pivot;
}args;

int cmpfunc(const void *a, const void *b) {
	const unsigned int ua = *(const unsigned int*)a;
	const unsigned int ub = *(const unsigned int*)b;
	if(ua == ub) return 0;
	else if (ua < ub) return -1;
	else return 1;
}

void* rearrangement(void* arg) {
	args *info = arg;
	int n = info->n;
	printf("\n");
	int pivot = info->pivot;
	int thread_part = n/MAXTHREADS;
	int *local_arr = malloc(sizeof(int) * thread_part);
	int cnt = 0;
	/*printf("Curr thread: %d A: ", current_thread);
	for(size_t i = 0; i < n; i++)
	{
		printf("%d  ", A[i]);
	}
	printf("\n");*/
	for (int i = current_thread * thread_part; i < (current_thread + 1) * thread_part  && cnt < thread_part; ++i) {
		
		local_arr[cnt] = A[i];
		printf("thread [%d] A[%d], local_arr: %d\n", current_thread, i,local_arr[cnt]);
		if (A[i] <= pivot) {
			//partition(local_arr, 0, thread_part);
		}
		cnt++;
		
	}
	printf("\n");
	
	printf("THREAD %d, local_arr: ", current_thread);
	for(size_t i = 0; i < thread_part; i++)
	{
		printf("%d ", local_arr[i]);
	}
	printf("\n");

	int k = 0;
	for (int j = 0; j < thread_part-1; j++){
        
        // If current element is smaller than or
        // equal to pivot
        if (local_arr[j] <= pivot){
            k++;
            swap(local_arr, k, j);
        }
    }

	printf("THREAD %d, NEW local_arr: ", current_thread);
	for(size_t i = 0; i < thread_part; i++)
	{
		printf("%d ", local_arr[i]);
	}
	printf("\n");
	
	//qsort(local_arr, n, sizeof(int), cmpfunc);
	free(local_arr);
	current_thread++;
	pthread_exit(0);
}




int main(int argc, char const *argv[]) {
	
	n = 24;
	A = malloc(sizeof(int)*n);
	int arr[] = {7,13,18,2,17,1,14,20,6,10,15,9,22,3,16,19,4,23,11,12,5,8,21,24};
	
	for(size_t i = 0; i < n; i++){
		A[i] = arr[i];
	}
	

	MAXTHREADS = sysconf(_SC_NPROCESSORS_ONLN);
	printf("Cores available: %d\n", MAXTHREADS);
	
	int pivot = 7;
	pthread_t threads[MAXTHREADS];
	
	int chunk = n/MAXTHREADS;
	for (int i = 0; i < n; ++i) {
		if (i%chunk == 0) {
			printf("|");
		}
		printf(" %d ", arr[i]);
	}
	printf("|\n\n");

	args *info = malloc(sizeof(args));
	info->n = n;
	info->pivot = pivot;
	
	for (int i = 0; i < MAXTHREADS; ++i) {
		pthread_mutex_lock(&lock);
		if (pthread_create(&threads[i], NULL, (void*)rearrangement, info)) {
			free(info);
			pthread_join(threads[i], NULL);
		}
		pthread_mutex_unlock(&lock);
	}
	
	return 0;
}