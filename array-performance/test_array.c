#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SIZE 2048 

int main() {
    
    struct timespec start, end;
    double cpu_time_used;
    int i, j;

    char arr[MAX_SIZE][MAX_SIZE];

    clock_gettime(CLOCK_MONOTONIC, &start);

    for(i = 0; i < MAX_SIZE; i++) {
        for(j = 0; j < MAX_SIZE; j++) {
            arr[i][j] = 0;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    cpu_time_used = (end.tv_nsec - start.tv_nsec) / 10e9;

    printf("row-column: %fs\n", cpu_time_used);

    clock_gettime(CLOCK_MONOTONIC, &start);

    for(i = 0; i < MAX_SIZE; i++) {
        for(j = 0; j < MAX_SIZE; j++) {
            arr[j][i] = 0;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    cpu_time_used = (end.tv_nsec - start.tv_nsec) / 10e9;

    printf("column-row: %fs\n", cpu_time_used);

    return 0;
    
}