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

UINT *readbuf;

void swap(UINT *A, int a, int b) {
    UINT tmp;
    tmp = A[a]; 
    A[a] = A[b]; 
    A[b] = tmp;
}

int partition (UINT* A, int lo, int hi){
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

// TODO: implement
typedef struct {
    unsigned int *array;
    int low;
    int high;
} arguments;

int parallel_quicksort(UINT* A, int lo, int hi) {
    // se def num de threads p 
    int n_threads = sysconf(_SC_NPROCESSORS_ONLN);
    pthread_t threads[n_threads];

    // largo de cada particion
    int size_of_partitions = sizeof(A)/n_threads;
    
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
    // de manera q en la mitad izq quedan los valrs menores del
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

int main(int argc, char** argv) {
    printf("[quicksort] Starting up...\n");

    /* Get the number of CPU cores available */
    printf("[quicksort] Number of cores available: '%ld'\n",
           sysconf(_SC_NPROCESSORS_ONLN));

    /* TODO: parse arguments with getopt */
    int E_value;
    int T_value;

    int index;
    int c;

    opterr = 0;
    char  char_T_val[2] = "";
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

    char *begin = "BEGIN U ";
    strcat(begin, char_T_val);
    /* DEMO: request two sets of unsorted random numbers to datagen */
    for (int i = 0; i < 2; i++) {
        /* T value 3 hardcoded just for testing. */
        int rc = strlen(begin);

        /* Request the random number stream to datagen */
        if (write(fd, begin, strlen(begin)) != rc) {
            if (rc > 0) fprintf(stderr, "[quicksort] partial write.\n");
            else {
                perror("[quicksort] write error.\n");
                exit(-1);
            }
        }

        /* validate the response */
        char respbuf[10];
        read(fd, respbuf, strlen(DATAGEN_OK_RESPONSE));
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
        
        /*aqui va el quicksort*/

        clock_gettime(CLOCK_MONOTONIC, &finish);
        /* Probe time elapsed. */
        elapsed = (finish.tv_sec - start.tv_sec);
        elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;


        struct timespec start_2, finish_2;
        double elapsed_2 = 0;
        clock_gettime(CLOCK_MONOTONIC, &start_2);
        
        /*aqui va quicksort*/

        clock_gettime(CLOCK_MONOTONIC, &finish_2);
        elapsed_2 = (finish_2.tv_sec - start_2.tv_sec);
        elapsed_2 += (finish_2.tv_sec - start_2.tv_sec) / 1000000000.0;

        printf("%d,%d,%lf,%lf\n", E_value, T_value, elapsed_2, elapsed);
    }
    free(readbuf);
    close(fd);
    exit(0);
}
