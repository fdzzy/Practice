#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define THREAD_NUM 5

void* busyWork(void *t)
{
    int i;
    long tid;
    double result = 0.0;
    tid = (long)t;

    printf("Thread %ld starting...\n", tid);
    
    for(i=0 ; i<1000000 ; i++) {
        result += sin(i) * tan(i);
    }

    printf("Thread %ld done. Result = %e\n", tid, result);
    pthread_exit((void*) t);
}

int main() {
    pthread_t threads[THREAD_NUM];
    pthread_attr_t attr;
    int rc;
    long t;
    void *status;

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(t=0 ; t<THREAD_NUM ; t++) {
        printf("Main: creating thread %ld\n", t);
        rc =  pthread_create(&threads[t], &attr, busyWork, (void*) t);
        if(rc) {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for(t=0; t<THREAD_NUM ; t++) {
        rc = pthread_join(threads[t], &status);
        if(rc) {
            printf("ERROR: return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        printf("Main: completed join with thread %ld having a status of %ld\n", t, (long)status);
    }

    printf("Main: program completed. Exiting...\n");
    pthread_exit(NULL);
}
