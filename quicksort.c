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
#include "omp.h"
#include <signal.h>
UINT *readbuf;
//UINT *readbuf_copy;
int MAXTHREADS;

// TODO: implement
void quicksort (UINT* arr, int left, int right) {
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


void quickSort_parallel_internal(UINT* array, int left, int right, int cutoff) 
{   
    int i = left, j = right;
    int tmp;
    int pivot = array[(left + right) / 2];    
    {
        /* PARTITION PART */
        while (i <= j) {
            while (array[i] < pivot)
                i++;
            while (array[j] > pivot)
                j--;
            if (i <= j) {
                tmp = array[i];
                array[i] = array[j];
                array[j] = tmp;
                i++;
                j--;
            }
        }

    }
    if ( ((right-left)<cutoff) ) {
        if (left < j) { 
            quickSort_parallel_internal(array, left, j, cutoff); 
        }           
        if (i < right) { 
            quickSort_parallel_internal(array, i, right, cutoff); 
        }
    }
    else{
        #pragma omp task 
        { quickSort_parallel_internal(array, left, j, cutoff); }
        #pragma omp task 
        { quickSort_parallel_internal(array, i, right, cutoff); }       
    }

}

void quickSort_parallel(UINT* array, int lenArray, int numThreads){
    int cutoff = 1000;
    #pragma omp parallel num_threads(numThreads)
    {   
        #pragma omp single nowait
        {
            quickSort_parallel_internal(array, 0, lenArray-1, cutoff);  
        }
    }   

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
        printf ("[quicksort] Non-option argument %s\n", argv[index]);

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
    /* Create the domain socket to talk to datagen. */
    struct sockaddr_un addr;
    int fd;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("[quicksort] Socket error.\n");
        exit(-1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, DSOCKET_PATH, sizeof(addr.sun_path) - 1);

    while (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("[quicksort] connect error.\n");
        close(fd);
        exit(-1);
    }

    if ( connect(fd, (struct sockaddr*)&addr, sizeof(addr)) != -1){
        printf("[quicksort] connected");
    }


    char begin[] = "BEGIN U ";
    strcat(begin, char_T_val);
    /* DEMO: request two sets of unsorted random numbers to datagen */
    for (int i = 0; i < 2; i++) {
        int rc = strlen(begin);

        if (write(fd, begin, strlen(begin)) != rc) {
            if (rc > 0) fprintf(stderr, "[quicksort] partial write.\n");
            else {
                perror("[quicksort] write error.\n");
                exit(-1);
            }
        }

        /* validate the response */
        char respbuf[10];
        
        //read(fd, respbuf, strlen(DATAGEN_OK_RESPONSE));
        respbuf[strlen(DATAGEN_OK_RESPONSE)] = '\0';

        if (strcmp(respbuf, DATAGEN_OK_RESPONSE)) {
            perror("[quicksort] Response from datagen failed.\n");
            close(fd);
            exit(-1);
        }

        UINT readvalues = 0;
        size_t numvalues = pow(10, 3);
        size_t readbytes = 0;

        UINT *readbuf = malloc(sizeof(UINT) * numvalues);
        //UINT *readbuf_copy = malloc(sizeof(UINT) * numvalues);


        while (readvalues < numvalues) {
            /* read the bytestream */
            readbytes = read(fd, readbuf + readvalues, sizeof(UINT) * 1000);
            readvalues += readbytes / 4;
        }

        /* Print out the values obtained from datagen */
        for (UINT *pv = readbuf; pv < readbuf + numvalues; pv++) {
            printf("%u\n", *pv);
        }

    }

    /* Issue the END command to datagen */
    int rc = strlen(DATAGEN_END_CMD);
    if (write(fd, DATAGEN_END_CMD, strlen(DATAGEN_END_CMD)) != rc) {
        if (rc > 0) fprintf(stderr, "[quicksort] partial write.\n");
        else {
            perror("[quicksort] write error.\n");
            close(fd);
            exit(-1);
        }
    }



    for(int i = 0; i < E_value; i++) {
        struct timespec start, finish;
        double elapsed = 0;
        clock_gettime(CLOCK_MONOTONIC, &start);
        quicksort(readbuf, 0, pow(10, T_value) - 1);

        clock_gettime(CLOCK_MONOTONIC, &finish);
        /* Probe time elapsed. */
        elapsed = (finish.tv_sec - start.tv_sec);
        elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;


        struct timespec start_2, finish_2;
        double elapsed_2 = 0;
        clock_gettime(CLOCK_MONOTONIC, &start_2);
        
        quickSort_parallel(readbuf, pow(10, T_value), MAXTHREADS);

        clock_gettime(CLOCK_MONOTONIC, &finish_2);
        elapsed_2 = (finish_2.tv_sec - start_2.tv_sec);
        elapsed_2 += (finish_2.tv_sec - start_2.tv_sec) / 1000000000.0;
    }
    printf("[quicksort] finished\n");
    free(readbuf);
    //free(readbuf_copy);
    close(fd);
    exit(0);
}
