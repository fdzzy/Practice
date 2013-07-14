#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void* thr_func1(void* arg) {
    printf("1: In thread function 1\n1: Getting lock1...\n");
    pthread_mutex_lock(&lock1);
    printf("1: Get lock1\n1: Getting lock2...\n");
    pthread_mutex_lock(&lock2);
    printf("1: Get lock2\n1: Doing some task...\n1: Releasing lock2...\n");
    pthread_mutex_unlock(&lock2);
    printf("1: Lock2 released!\n1: Releasing lock1...\n"); 
    pthread_mutex_unlock(&lock1); 
    printf("1: Done!\n");
} 

void* thr_func2(void* arg) {
    printf("2: In thread function 2\n2: Getting lock2...\n");
    pthread_mutex_lock(&lock2);
    printf("2: Get lock2\n2: Getting lock1...\n");
    pthread_mutex_lock(&lock1);
    printf("2: Get lock2\n2: Doing some task...\n2: Releasing lock1...\n");
    pthread_mutex_unlock(&lock1);
    printf("2: Lock1 released!\n2: Releasing lock2...\n"); 
    pthread_mutex_unlock(&lock2); 
    printf("2: Done!\n");
} 

int main(int argc, char** argv) {
    int err;
    pthread_t tid1, tid2;
    void *rtval;
    
    err = pthread_create(&tid1, NULL, thr_func1, NULL); 
    if(err != 0) {
        exit(-1);
    }
    err = pthread_create(&tid2, NULL, thr_func2, NULL);
    if(err != 0) {
        exit(-1);
    }

    err = pthread_join(tid1, &rtval);
    if (err != 0) {
        exit(-1);
    }

    err = pthread_join(tid2, &rtval);
    if (err != 0) {
        exit(-1);
    }

    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);
    

    exit(0);
}
