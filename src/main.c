#include  <pthread.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <stdio.h>
#include  <sys/types.h>
#include  <fcntl.h>
#include  <string.h>
#include  <sys/stat.h>
#include  <semaphore.h>
#include  <sys/time.h>
#include <stdbool.h>

#define BUFFER_SIZE 255

/* --- Structs --- */

typedef struct ThreadParams {
    int pipeA[2];
    int pipeB[2];
    sem_t sem_A, sem_B, sem_C;
} ThreadParams;

pthread_attr_t attr;
FILE *input_file, *output_file;


/* --- Prototypes --- */

/* Initializes data and utilities used in thread params */
void initializeData(ThreadParams *params);

/* This thread reads data from data.txt and writes each line to a pipe */
void* ThreadA(void *params);

/* This thread reads data from pipe used in ThreadA and writes it to a shared variable */
void* ThreadB(void *params);

/* This thread reads from shared variable and outputs non-header text to src.txt */
void* ThreadC(void *params);

/* --- Main Code --- */
int main(int argc, char const *argv[]) {
    int err;
    pthread_t tid[3];
    pthread_attr_t attr;
    ThreadParams params;

    // Open input file and output file
    input_file = fopen("data.txt", "r");
    output_file = fopen("out.txt", "w");

    // Initialization
    initializeData(&params);

    // Create Threads
    err += pthread_create(&(tid[0]), &attr, &ThreadA, (void*)(&params));
    err += pthread_create(&(tid[1]), &attr, &ThreadB, (void*)(&params));
    err += pthread_create(&(tid[2]), &attr, &ThreadC, (void*)(&params));
    if (err != 0) {  // Handle thread creation failing
        perror("Failed to create threads");
    }

    // Wait on threads to finish
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);

    // Close input file and output file
    fclose(input_file);
    fclose(output_file);

    return 0;
}

void initializeData(ThreadParams *params) {
    // Initialize Sempahores
    sem_init(&(params->sem_A), 0, 1);
    sem_init(&(params->sem_B), 0, 0);
    sem_init(&(params->sem_C), 0, 0);

    // Initialize thread attributes 
    pthread_attr_init(&attr);

    // Create Pipes
    if (pipe(params->pipeA) == -1 || pipe(params->pipeB) == -1) {
        perror("Failed to create pipes");  // Handle pipe creation failing
    }

    return;
}

void* ThreadA(void *params) {
    char buffer[BUFFER_SIZE];  // Create buffer to read data
    ThreadParams *threadParams = (ThreadParams*)params;  // Cast params to ThreadParams struct

    // Iterate through the data.txt file
    while (fgets(buffer, BUFFER_SIZE, input_file) != NULL) {
        // Wait for semaphore availability
        sem_wait(&(threadParams->sem_A));

        // Write line into pipeA
        write(threadParams->pipeA[1], buffer, strlen(buffer));
        memset(buffer, '\0', sizeof(buffer));
        printf("\nthread A read from data.txt\n");

        // Signal to ThreadB semaphore availability
        sem_post(&(threadParams->sem_B));
    }
    
    // Close pipe and thread
    close(threadParams->pipeA[1]);
    pthread_exit(NULL);
}

void* ThreadB(void *params) {
    char buffer[BUFFER_SIZE];  // Create buffer to read data
    int bytes_read;  // Create variable to store the amount of data read
    ThreadParams *threadParams = (ThreadParams*)params;  // Cast params to ThreadParams struct


    // Iterate through the data in pipeA
    while ((bytes_read = read(threadParams->pipeA[0], buffer, BUFFER_SIZE)) > 0) {
        // Wait for semaphore availability
        sem_wait(&(threadParams->sem_B));

        // Write the pipeA data into pipeB
        write(threadParams->pipeB[1], buffer, bytes_read);
        memset(buffer, '\0', sizeof(buffer));
        printf("thread B read from data.txt\n");  

        // Signal to ThreadC semaphore availability
        sem_post(&(threadParams->sem_C));
    }
    
    // Close pipes and thread
    close(threadParams->pipeA[0]);
    close(threadParams->pipeB[1]);
    pthread_exit(NULL);
    
}

void* ThreadC(void *params) {
    char buffer[BUFFER_SIZE];  // Create buffer to read data
    int bytes_read;  // Create variable to store the amount of data read
    bool header_found = false;  // Create flag for finding the end of header
    ThreadParams *threadParams = (ThreadParams*)params;  // Cast params to ThreadParams struct


    // Iterate through the data in pipeB
    while ((bytes_read = read(threadParams->pipeB[0], buffer, BUFFER_SIZE)) > 0) {
        // Wait for semaphore availability
        sem_wait(&(threadParams->sem_C));

        // If the end of header has been reached, write to out.txt
        printf("Line in buffer: %s", buffer);
        if (header_found) {
            fwrite(buffer, sizeof(char), bytes_read, output_file);  // Write to file 
        } else if ((strcmp(buffer, "end_header\n") == 0)) {
            header_found = true;  // Flag end of header found
        }
        memset(buffer, '\0', sizeof(buffer));
        printf("thread C read from data.txt and wrote into out.txt\n");

        // Signal to ThreadA semaphore availability, restarting A->B->C sequential loop for the next line in data.txt
        sem_post(&(threadParams->sem_A));
    }

    // CLose pipe and thread
    close(threadParams->pipeB[0]);
    pthread_exit(NULL);
}
