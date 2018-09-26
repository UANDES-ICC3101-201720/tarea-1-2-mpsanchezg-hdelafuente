#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>
#include <limits.h>
#include <signal.h>

int MAXTHREADS;
int n;
int current_thread = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


unsigned int *A;

void swap(unsigned int *A, int a, int b) {
    unsigned int tmp;
    tmp = A[a]; 
    A[a] = A[b]; 
    A[b] = tmp;
}

int partition (unsigned int* A, int lo, int hi) {
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
int quicksort (unsigned int* A, int lo, int hi) {
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
    int n;
    int pivot;
    int n_less_than_pivot;
} args;


pthread_t *separate_pthreads(pthread_t* threads, int first, int last){
    pthread_t *new_array_pthrads;

    new_array_pthrads = malloc(sizeof(UINT)*(last-first));

    for(size_t i = first; i <= last; i++)
    {
        new_array_pthrads[i] = threads[i];
    }
    return new_array_pthrads;
}

void* parallel_partition(void *arg){
    args *info = arg;
    int n = info->n;
    int pivot = info->pivot;

    int thread_part = n/MAXTHREADS;
    int cnt = 0;

    unsigned int *local_arr = malloc(sizeof(int) * thread_part);
    
    for (int i = current_thread * thread_part; i < (current_thread + 1) * thread_part  && cnt < thread_part; ++i) {
        local_arr[cnt] = A[i];
        cnt++;
    }
    
    int k = -1;
    for (int j = 0; j <= thread_part; j++) {
        if (local_arr[j] <= pivot){
            k++;
            swap(local_arr, k, j);
        }
    }
   
    k++; //numero de valores menores o iguales al pivote

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

    free(local_arr);
    current_thread++;
    pthread_exit(0);
}

int parallel_quicksort(unsigned int* A, int lo, int hi, pthread_t threads[], int q_threads, int size_arr){
    int pivot = (hi + lo) / 2;
    /* cuando el mutex este en 1 */
    args *info = malloc(sizeof(args));
    info->n = n;
    info->pivot = pivot;
    for (int i = 0; i < MAXTHREADS; ++i) {
        pthread_mutex_lock(&lock);
        if (pthread_create(&threads[i], NULL, (void*)parallel_partition, info)) {
            free(info);         
        }
        pthread_join(threads[i], NULL);
        pthread_mutex_unlock(&lock);
    }

    pthreads_t threads_to_left[], threads_to_right[];
    int q_to_left; // ultimo del S_i
    
    threads_to_left = separate_pthreads(threads, first, q_to_left);
    threads_to_left = separate_pthreads(threads, q_to_left, hi - q_to_left);

    parallel_quicksort(A, lo, last_positioned, threads_to_right, q_threads_to_right);
    parallel_quicksort(A, last_positioned + 1, hi, threads_to_left, q_threads - q_threads_to_right);

    return 0;
}

int main(int argc, char** argv){
    return 0;
}