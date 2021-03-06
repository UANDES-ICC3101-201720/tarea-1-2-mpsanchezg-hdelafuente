#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "types.h"
#include "const.h"
#include "util.h"
#include <ctype.h>
#include <pthread.h>
#include <limits.h>
#include <signal.h>

void swap(UINT *A, int a, int b) {
    UINT tmp;
    tmp = A[a]; 
    A[a] = A[b]; 
    A[b] = tmp;
}

int partition (UINT* A, int lo, int hi) {
    int pivot = A[hi]; // pivot
    int i = (lo - 1);  // Index of smaller element
    
    for (int j = lo; j <= hi-1; j++) {
        
        // If current element is smaller than or
        // equal to pivot
        if (A[j] <= pivot) {
            i++;
            swap(A, i, j);
        }
    }
    swap(A, i + 1, hi);
    return (i + 1);
}

// TODO: implement
int quicksort (UINT* A, int lo, int hi) {
    if (lo < hi){
        /* pi es partitioning index, arr[p] is now
           at right place */
        int pi = partition(A, lo, hi);
        
        // Separately sort elements before
        // partition and after partition
        quicksort(A, lo, pi - 1);
        quicksort(A, pi + 1, hi);
    }
    return 0;
}


typedef struct {
    UINT *array;
    int low;
    int high;
    int pivot;
    int n_less_than_pivot;
    int n_thread;
} arguments;


void* parallel_partition(void *args){
    arguments *information = args;
    int lo, hi, piv, nums_less_than_pivot, thr;
    UINT *A;

    lo  =      information->low;
    hi  =     information->high;
    piv =    information->pivot;
    A   =    information->array;
    thr = information->n_thread;

    nums_less_than_pivot = information->n_less_than_pivot;

	int thread_part = n/MAXTHREADS; // cambiar n por el largo de A
	unsigned int *local_arr = malloc(sizeof(int) * thread_part);
	int cnt = 0;
	for (int i = current_thread * thread_part; i < (current_thread + 1) * thread_part  && cnt < thread_part; ++i) {
		local_arr[cnt] = A[i];
		cnt++;		
	}
	
	int k = -1;
	for (int j = 0; j <= thread_part; j++){
        if (local_arr[j] <= pivot){
			k++;
            swap(local_arr, k, j);
        }
    }
    k++;
    
    /* 2. Guardar la cantidad en un arreglo |S_i| */
    // (hacer una variable global que se pueda hacer malloc y free)

    /* 3. Guardar posiciones en prefix sum */
    // Empieza de cero y se suma con el primer numero del arreglo anterior
    
    /* 4. Posicionar los numeros encontrados segun prefix sum */
    // No se si hacer un arreglo nuevo o ponerlo en el mismo arreglo
    // pero el tema es que hay que saber la posicion actual y la futura

    /* 2, 3 y 4 se debe hacer junto con los demás threads. La idea es que según el 
       numero de thread, que se posicione la cantidad en |S_i| */
    /* --- de alguna forma hay que usar variables de condicion, un thread jefe y un mutex --- */        
}

int q_threads_per_array(int len_old_array, int len_new_array){


}


int parallel_quicksort(UINT* A, int lo, int hi, pthreads_t threads[], int q_threads, int size_arr){

    /* cuando el mutex este en 1 */

    int last_positioned;
    int q_threads_to_right;

    /* hacer que los threads lean su parte del arreglo */
    for(int i = 0; i < q_threads; i++){
        /* threads leyendo numeros menores al pivote */
        /* cambiarlo a pthread_create y poner los attr en un struct */
        int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          (void *) parallel_partition, void *arg);

        // parallel_partition(A, size_arr * i , size_arr * (i + 1) - 1, threads[i], pivot);
        
    }

    pthreads_t threads_to_left[], threads_to_right[];

    parallel_quicksort(A, lo, last_positioned, threads_to_right, q_threads_to_right);
    parallel_quicksort(A, last_positioned + 1, hi, threads_to_left, q_threads - q_threads_to_right);

    return 0;
}

int main(int argc, char** argv){
    return 0;
}