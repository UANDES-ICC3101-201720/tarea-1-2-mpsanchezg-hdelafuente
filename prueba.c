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

int partition (UINT* A, int lo, int hi){
    int pivot = arr[hi]; // pivot
    int i = (lo - 1);     // Index of smaller element
    
    for (int j = lo; j <= hi-1; j++){
        
        // If current element is smaller than or
        // equal to pivot
        if (A[j] <= pivot){
            i++;
            swap(&A[i], &A[j]);
        }
    }
    swap(&A[i + 1], &A[hi]);
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
    unsigned int *array;
    int low;
    int high;
} arguments;

int parallel_quicksort(UINT* A, int lo, int hi) {
    // se def num de threads p 
    int n_threads = sysconf(_SC_NPROCESSORS_ONLN);
    pthread_t threads[nThreads];

    // largo de cada particion
    int size_of_partitions = sizeof(A)/p;
    
    int count = 1;
    for (int i = 0; i < n_threads; i++){
        arguments *information = malloc(sizeof(arguments));
        information->array = A;
        information->low = lo;
        information->high = (size_of_partitions * count) - 1;
        /*
        if (pthread_create(&threads[i], NULL, (void *)quicksort, information)){
            free(information);
        }*/
        lo = size_of_partitions * count;
        count ++;

    }
    // A_i subarreglo o bloque asignado al proceso P_i
    
    // Un thread elige un valor pivote aleatorio dentro de A
    // y lo comunica a todos los demas threads

    // Cada proceso P_i mueve los valores en su bloque A_i
    // de manea q en la mitad izq quedan los valrs menores del
    // pivote (sub-bloque S_i), y en el lado der los val may a 
    // (sub-bloque  L_i). Esto es similar a lo que hace el
    // procedimiento partition dentro de su ciclo en el alg quicksort serial.

    // Se hace reordenamiento global de los ítems del arreglo A
    // creando un arreglo A', en donde se copian los elementos
    // en orden descrito (ver fig 9.19 en libro)

    // El arreglo A' tiene dos secciones; una que contiene los valores
    // al pivote, S,  otra seccion contigua a la der que contiene los
    // valores mayores, L.

    // El algoritmo se repite recursivamente, asignando los primeros 
    // [|S|p/n+0,5] threads a  la seccion S los threads restantes a la 
    // seccion L.

    // La recursión termina cuando un sub-bloque de A con todos los valores
    // mayores (o menores) al pivote asignado a un proceso. En este caso, 
    // el proceso ejecuta la versión serial del algoritmo Quicksort sobre
    // el  bloque que recibe.


    return 0;
}