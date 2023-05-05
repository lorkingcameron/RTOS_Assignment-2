/***********************************************************************************/
//***********************************************************************************
//            *************NOTE**************
// This is a template for the subject of RTOS in University of Technology Sydney(UTS)
// Please complete the code based on the assignment requirement.

//***********************************************************************************
/***********************************************************************************/

/*
  To compile prog_1 ensure that gcc is installed and run the following command:
  gcc prog_1.c -o prog_1 -lpthread -lrt -Wall

*/
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

/* --- Structs --- */

typedef struct ThreadParams {
  int pipeFile[2];
  sem_t sem_A, sem_B, sem_C;
  char message[255];
  } ThreadParams;

pthread_attr_t attr;

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
  //pthread_attr_t attr;
  ThreadParams params;
  

  // Initialization
  initializeData(&params);
  

  // Create Threads
  pthread_create(&(tid[0]), &attr, &ThreadA, (void*)(&params));


  //TODO: add your code
 

  // Wait on threads to finish
  pthread_join(tid[0], NULL);
  
  //TODO: add your code

  return 0;
}

void initializeData(ThreadParams *params) {
  // Initialize Sempahores
  sem_init(&(params->sem_A), 0, 1);
 // Initialize thread attributes 
  pthread_attr_init(&attr);
  //TODO: add your code

  return;
}

void* ThreadA(void *params) {
  //TODO: add your code
  printf("\nthread A read from data.txt\n");
}

void* ThreadB(void *params) {
  //TODO: add your code
  printf("\nthread B read from data.txt\n");
}

void* ThreadC(void *params) {
  //TODO: add your code
 printf("\nthread C read from data.txt\n");
}
