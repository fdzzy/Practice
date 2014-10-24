/* 
Problem: create 3 threads named A, B, C which prints out "A", "B", "C" respectly. And they will each print out 10 times. The output should be "ABCABCABC..."
*/
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define COUNT 10 

sem_t sem_ab;
sem_t sem_bc;
sem_t sem_ca;

void* a(void* arg) {
    int i;
    printf("Thread A starting...\n");
    for(i=0; i<COUNT ; i++) {
        sem_wait(&sem_ca);
        printf("A");
        sem_post(&sem_ab);
    }
    pthread_exit(NULL);
}

void* b(void* arg) {
    int i;
    printf("Thread B starting...\n");
    for(i=0 ; i<COUNT ; i++) {
        sem_wait(&sem_ab);
        printf("B");
        sem_post(&sem_bc);
    }
    pthread_exit(NULL);
}

void* c(void* arg) {
    int i;
    printf("Thread C starting...\n");
    for(i=0 ; i<COUNT ; i++) {
        sem_wait(&sem_bc);
        printf("C\n");
        sem_post(&sem_ca); 
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t thrs[3];

    printf("Main thread starting...\n");

    sem_init(&sem_ab, 0, 0);
    sem_init(&sem_bc, 0, 0);
    sem_init(&sem_ca, 0, 1);

    pthread_create(&thrs[0], NULL, a, NULL);
    pthread_create(&thrs[1], NULL, b, NULL);
    pthread_create(&thrs[2], NULL, c, NULL);

    pthread_join(thrs[0], NULL);
    pthread_join(thrs[1], NULL);
    pthread_join(thrs[2], NULL);

    sem_destroy(&sem_ca);
    sem_destroy(&sem_bc);
    sem_destroy(&sem_ab);
    printf("Main thread exiting...\n");

    return 0;
}
