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
#define CON_CT 8 
// Producer count 
#define PRO_CT 8 
// BUFFER Size
#define BUF_SIZE 5 

sem_t p_mutex;
int p_count = 0;
sem_t c_mutex;
int c_count = 0;

// semaphor to coordinate producer/consumer
sem_t sem_con;
sem_t sem_pro;

void* consumer(void* id) {
    int num = (int)id;
    int i = 0;
    int count;

    printf("\tConsumer %d starting...\n", num);

    while(1) {
        sem_wait(&c_mutex);
        if(c_count == (CON_CT*PRO_CT)) {
            sem_post(&c_mutex);
            break;
        }
        /* Waiting for products */
        sem_wait(&sem_con);
        count = c_count;
        printf("\tConsumer %d start consuming product #%d, my #%d\n", num, count, i);
        c_count++;
        sem_post(&c_mutex);
        /* Simulating the time to consuming */
        if(num%2) {
            sleep(5);
        } else {
            sleep(3);
        }
        printf("\tConsumer %d finished consuming product #%d, my #%d\n", num, count, i++);
        /* Signal producer */
        sem_post(&sem_pro);
    }
    printf("\tConsumer %d consumed %d product(s), exiting...\n", num, i);

    pthread_exit(NULL);
}

void* producer(void* id) {
    int num = (int)id;
    int i;

    printf("Producer %d starting...\n", num);
    for(i=0; i<CON_CT ; i++) {
        sleep(1);
        /* Waiting for buffer size */
        sem_wait(&sem_pro);
        sem_wait(&p_mutex);
        printf("Producer %d produced product #%d, my #%d\n", num, p_count, i);
        p_count++;
        sem_post(&p_mutex);
        /* Signal consumer */
        sem_post(&sem_con);
    }
    printf("Producer %d produced %d product(s), exiting...\n", num, i);

    pthread_exit(NULL);
}

int main() {
    int i;
    pthread_t con_thrs[CON_CT];
    pthread_t pro_thrs[PRO_CT];

    printf("Main thread starting...\n");

    sem_init(&p_mutex, 0, 1);
    sem_init(&c_mutex, 0, 1);
    // initialize consumer semaphor to be zero indicating no products
    sem_init(&sem_con, 0, 0);
    // initialize producer semaphor to be BUF_SIZE indicating BUF_SIZE poducts can be produced
    sem_init(&sem_pro, 0, BUF_SIZE);

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

    sem_destroy(&sem_pro);
    sem_destroy(&sem_con);
    sem_destroy(&c_mutex);
    sem_destroy(&p_mutex);
    printf("Main thread exiting...\n");

    return 0;
}
