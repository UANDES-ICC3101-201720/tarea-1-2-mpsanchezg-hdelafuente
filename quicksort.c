#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
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

UINT *nums;
UINT *nums_copy;
int MAXTHREADS;
int n;

void SWAP(UINT *A, int a, int b) {
    UINT tmp;
    tmp = A[a]; 
    A[a] = A[b]; 
    A[b] = tmp;
}

int partition(UINT *arr, int l, int h, int pivot)
{
    int pivotValue = arr[pivot];
    SWAP(arr, pivot, h);
    int storeIndex = l;
    for (int i=l ; i<h ; i++)
    {
        if (arr[i] <= pivotValue)
        {
            SWAP(arr, i, storeIndex);
            storeIndex++;
        }
    }
    SWAP(arr, storeIndex, h);
    return storeIndex;
}

// TODO: implement
void quicksort (UINT* arr, int lo, int hi) {
    int i = lo, j = hi;
    int tmp;
    int pivot = arr[(lo + hi) / 2];
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
    if (lo < j){ quicksort(arr, lo, j);  }
    
    if (i< hi){ quicksort(arr, i, hi); }
}

// TODO: implement
struct args {
    UINT *arr;
    int l;
    int h;
    int threads;
};

void parallel_quicksort(UINT *arr, int l, int h, int threads);

void* p_quicksort(void *arg) {
    struct args *info = arg;
    parallel_quicksort(info->arr, info->l, info->h, info->threads);
    return NULL;
}

void parallel_quicksort(UINT *arr, int l, int h, int threads) {
    if (h > l) {
        int pIndex = l + (h - l)/2;
        pIndex = partition(arr, l, h, pIndex);

        if (threads-- > 0) {
            struct args arg = {arr, l, pIndex-1, threads};
            pthread_t thread;
            int ret = pthread_create(&thread, NULL, p_quicksort, &arg);
            assert((ret == 0) && "Thread creation failed");
            parallel_quicksort(arr, pIndex+1, h, threads);
            pthread_join(thread, NULL);
        }

        else {
            quicksort(arr, l, pIndex-1);
            quicksort(arr, pIndex+1, h);
        }
    }
}

void print_array(UINT *arr, int size) {
    for (int i = 0; i < size; ++i)
    {
        printf(" %d,", arr[i]);
    }
    printf("\n\n");
}

int main(int argc, char** argv) {
    MAXTHREADS = sysconf(_SC_NPROCESSORS_ONLN);
    printf("[quicksort] Starting up...\n");

    /* Get the number of CPU cores available */
    printf("[quicksort] Number of cores available: '%ld'\n",
           sysconf(_SC_NPROCESSORS_ONLN));

    /* TODO: parse arguments with getopt */
    int E_value = 1;
    int T_value = 3;

    int index;
    int c;

    opterr = 0;
    char char_T_val[2] = "";
    if (argc == 1) {
        fprintf(stderr, "[quicksort] No arguments where given\n");
        exit(-2);
    }

    while( (c = getopt(argc, argv, "E:T:P:")) != -1) {
        switch(c) {
            case 'E':
                E_value = atoi(optarg);
                break;
            case 'T':
                strcpy(char_T_val, optarg);
                T_value = atoi(optarg);
                break;
            case '?':
                if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default: 
                abort();
        }

    }

    if (E_value < 1) {
        fprintf(stderr, "[quicksort] Invalid value for E flag\n");
    }
    else if (T_value < 3 || T_value > 9) {
        fprintf(stderr, "[quicksort] Invalid value for T flag\n");
    }

    for (index = optind; index < argc; index++)
        fprintf (stderr, "[quicksort] Non-option argument %s\n", argv[index]);

    /* TODO: start datagen here as a child process. */
    int pid = fork();

    if (pid < 0) {
        perror("[quicksort] fork error.");
    }
    else if (pid == 0) {
        if (execv("./datagen", argv) < 0) {
            perror("[quicksort] execv error.");
            exit(-1);
        }
    }


    /*======================SOCKET============================*/
    for(int i = 0; i < E_value; i++) {
        struct sockaddr_un addr;
        int fd;
        n = pow(10, T_value); /*Amount of values in the array */

        if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1 ){
            perror("[quicksort] socket error");
            close(fd);
            exit(-1);
        }

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;


        strcpy(addr.sun_path, "/tmp/dg.sock");

        while ( connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1){
            perror("[quicksort] connect error");
        }

        if ( connect(fd, (struct sockaddr*)&addr, sizeof(addr)) != -1){
            printf("[quicksort] connected");
        }

        // begin
        char starting_instruction[] = "BEGIN U ";
        strcat(starting_instruction, char_T_val); //instruccion completa de inicio
        if (write(fd, starting_instruction, sizeof(starting_instruction)) == -1) {
            perror("[quicksort] Error giving starting_instruction.");
            close(fd);
            exit(-1);
        }
        // leo lo que me entrega el servidor
        long unsigned int readvalues = 0;
        size_t numvalues = pow(10, T_value);
        size_t readbytes = 0;

        UINT *nums = malloc(sizeof(UINT) * numvalues);
        UINT *nums_copy = malloc(sizeof(UINT) * numvalues);
        while(readvalues < numvalues) {
            readbytes = read(fd, nums + readvalues, sizeof(UINT)*1000);
            readvalues += readbytes / 4;
        }

        if(write(fd, DATAGEN_END_CMD, sizeof(DATAGEN_END_CMD)) == -1){
            // AQUI DEBERIA TERMINAR EL PROCESO DEL DATAGEN Y SEGUIR CON LO DEL OUTPUT...
            perror("[quicksort] Error terminating datagen.");
            close(fd);
            exit(-1);
        }

        for (int i = 0; i < n; ++i)
        {
            nums_copy[i] = nums[i];
        }
        
        //printf("E1: ");
        //print_array(nums, n);
        
        struct timespec start, finish;
        double elapsed = 0;
        clock_gettime(CLOCK_MONOTONIC, &start);
        
        quicksort(nums, 0, n-1);

        clock_gettime(CLOCK_MONOTONIC, &finish);

        elapsed = (finish.tv_sec - start.tv_sec);
        elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

        //printf("\nS1: ");
        //print_array(nums, n);

        struct timespec start_2, finish_2;
        double elapsed_2 = 0;
        clock_gettime(CLOCK_MONOTONIC, &start_2);
        
        parallel_quicksort(nums_copy, 0, n - 1, MAXTHREADS);


        clock_gettime(CLOCK_MONOTONIC, &finish_2);
        elapsed_2 = (finish_2.tv_sec - start_2.tv_sec);
        elapsed_2 += (finish_2.tv_sec - start_2.tv_sec) / 1000000000.0;
        
        printf("\nS2: ");
        print_array(nums_copy, n);
        
        printf("\n\nquicksort:  %lf [s]\n", elapsed);
        printf("parallel_quicksort:  %lf [s]\n", elapsed_2);
    }

    free(nums_copy);
    free(nums);
    exit(0);
}
