/*
This is a program to demonstrate the use of pthread semaphores
to implement the consumer/producer model 
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// Consumer count 
#define CON_CT 5 
// Producer count 
#define PRO_CT 3 

sem_t p_mutex;
int p_count = 0;
sem_t c_mutex;
int c_count = 0;

// semaphor to coordinate producer/consumer
sem_t sem;

void* consumer(void* id) {
    int num = (int)id;
    int i = 0;
    int count;

    printf("Consumer %d starting...\n", num);

    while(1) {
        sem_wait(&c_mutex);
        if(c_count == (CON_CT*PRO_CT)) {
            sem_post(&c_mutex);
            break;
        }
        sem_wait(&sem);
        count = c_count;
        printf("Consumer %d start consuming product #%d, my #%d\n", num, count, i);
        c_count++;
        sem_post(&c_mutex);
        if(num%2) {
            sleep(5);
        } else {
            sleep(3);
        }
        printf("Consumer %d finished consuming product #%d, my #%d\n", num, count, i++);
    }
    printf("Consumer %d consumed %d product(s), exiting...\n", num, i);

    pthread_exit(NULL);
}

void* producer(void* id) {
    int num = (int)id;
    int i;

    printf("Producer %d starting...\n", num);
    for(i=0; i<CON_CT ; i++) {
        sleep(1);
        sem_wait(&p_mutex);
        printf("Producer %d produced product #%d, my #%d\n", num, p_count, i);
        p_count++;
        sem_post(&p_mutex);
        sem_post(&sem);
    }
    printf("Producer %d produced %d product(s), exiting...\n", num, i);

    pthread_exit(NULL);
}

int main() {
    int i;
    pthread_t con_thrs[CON_CT];
    pthread_t pro_thrs[PRO_CT];

    printf("Main thread starting...\n");

    sem_init(&sem, 0, 0);
    sem_init(&p_mutex, 0, 1);
    sem_init(&c_mutex, 0, 1);

    for(i=0 ; i<CON_CT ; i++) {
        pthread_create(&con_thrs[i], NULL, consumer, (void*)i);
    }
    for(i=0 ; i<PRO_CT ; i++) {
        pthread_create(&pro_thrs[i], NULL, producer, (void*)i);
    }

    for(i=0 ; i<PRO_CT ; i++) {
        pthread_join(pro_thrs[i], NULL);
    }
    for(i=0 ; i<CON_CT ; i++) {
        pthread_join(con_thrs[i], NULL);
    }

    sem_destroy(&c_mutex);
    sem_destroy(&p_mutex);
    sem_destroy(&sem);
    printf("Main thread exiting...\n");

    return 0;
}
