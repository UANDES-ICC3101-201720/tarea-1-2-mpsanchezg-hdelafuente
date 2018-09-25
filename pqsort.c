l#include <stdio.h>
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
} arguments;

// *parallel_partition(UINT *A, int lo , int hi, pthread_t threads, int pivot)
void* parallel_partition(void *args){
    arguments *information = args;
    int lo, hi, piv;
    UINT *A;

    lo  =   information->low;
    hi  =  information->high;
    piv = information->pivot;
    A   = information->array;

    /* 1. Encontrar los numeros que son menores o iguales al pivote */

    /* 2. Guardar la cantidad en un arreglo |S_i| */

    /* 3. Guardar posiciones en prefix sum */

    /* 4. Posicionar los numeros encontrados segun prefix sum */

        
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

        parallel_partition(A, s ize_arr * i , size_arr * (i + 1) - 1, threads[i], pivot);
        
    }

    pthreads_t threads_to_left[], threads_to_right[];

    parallel_quicksort(A, lo, last_positioned, threads_to_right, q_threads_to_right);
    parallel_quicksort(A, last_positioned + 1, hi, threads_to_left, q_threads - q_threads_to_right);

    return 0;
}

int main(int argc, char** argv){
    return 0;
}