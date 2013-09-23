/*
This is a program to demonstrate the use of pthread condition variable
to make the main thread wait for multiple child threads to finish their tasks
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define THREAD_NUM 5 

pthread_mutex_t mutex;
pthread_cond_t cond;
int count = 0;

void* thread_body(void* arg) {
    int num = (int)arg;

    printf("thread %i starting...\n", num);

    // Do some job here
    if(num%2) {
        sleep(2);
    } else {
        sleep(3);
    }

    printf("thread %i finishes job\n", num);

    pthread_mutex_lock(&mutex);
    count++;
    if(count==THREAD_NUM) {
        pthread_cond_signal(&cond);
    }
    printf("thread %i exiting...\n", num);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main() {
    int i;
    pthread_t threads[THREAD_NUM];
    time_t start, finish;

    printf("Main thread starting...\n");

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    start = time(NULL);

    for(i=0 ; i<THREAD_NUM ; i++) {
        pthread_create(&threads[i], NULL, thread_body, (void*)i);
    }

    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

    finish = time(NULL);

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);

    printf("Main thread exiting...\n");
    printf("Time consumed: %d second(s)\n", (int)(finish - start));
    return 0;
}
