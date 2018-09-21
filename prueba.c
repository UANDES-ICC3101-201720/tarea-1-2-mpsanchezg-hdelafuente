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


int parallel_quicksort(UINT* A, int lo, int hi) {
    int p = sysconf(_SC_NPROCESSORS_ONLN);
    return 0;
}