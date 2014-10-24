/*
A prints "Shooting B #X" and shoots B, B does the same
If one thread gets 5 shots, it dies and creates a new thread to revenge
And generation should be at most 5
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#define INIT_HEALTH 5
#define GENERATION_CT 2 

typedef struct thread_arg_ {
    char name;
    char target;
    int gen;
    // used to calculate my health value, ie wait on this
    pthread_mutex_t *my_mutex;
    pthread_cond_t *my_cond;
    int *my_health; // protected by my_mutex
    // used to signal target to cal health value, ie signal on this
    pthread_mutex_t *target_mutex;
    pthread_cond_t *target_cond;
    int *target_health; // protected by target_mutex
}thread_arg_t;

typedef struct shooter_arg_ {
    char name;
    char target;
    int gen;
    int *exit_flag; // protected by exit_mutex
    // used to signal target to cal health value, ie signal on this
    pthread_mutex_t *target_mutex;
    pthread_cond_t *target_cond;
    int *target_health; // protected by target_mutex
    // used to signal target to cal health value, ie signal on this
    pthread_mutex_t *exit_mutex; 
}shooter_arg_t;

pthread_mutex_t A_health_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  A_cond = PTHREAD_COND_INITIALIZER;
int A_health = INIT_HEALTH; 
pthread_mutex_t B_health_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  B_cond = PTHREAD_COND_INITIALIZER;
int B_health = INIT_HEALTH; 
pthread_attr_t attr;

thread_arg_t A_arg = {'A', 'B', 1, &A_health_mutex, &A_cond, &A_health, &B_health_mutex, &B_cond, &B_health};
thread_arg_t B_arg = {'B', 'A', 1, &B_health_mutex, &B_cond, &B_health, &A_health_mutex, &A_cond, &A_health};

//// Shooter thread's responsibility is to shoot the other thread group
//void* shooter_thread(void* arg) {
//    shooter_arg_t *info = (shooter_arg_t*) arg;
//    int num = 0;
//
//    printf("\tShooter: %c generation #%d starting...\n", info->name, info->gen);
//
//    while(1) {
//        pthread_mutex_lock(info->exit_mutex);
//        if(*info->exit_flag) {
//            pthread_mutex_unlock(info->exit_mutex);
//            printf("\tShooter: %c generation #%d exiting...\n", info->name, info->gen);
//            pthread_exit(NULL);
//        }
//        pthread_mutex_unlock(info->exit_mutex);
//        //Shooting other
//        pthread_mutex_lock(info->target_mutex);
//        if(*info->target_health) {
//            printf("\tShooter: %c generation #%d shooting %c #%d\n", info->name, info->gen, info->target, num);
//            num++;
//            (*info->target_health)--;
//            pthread_cond_signal(info->target_cond);
//        }
//        pthread_mutex_unlock(info->target_mutex);
//        //sleep(1);
//    }
//
//    // Should never get here
//    assert(1 == 0);
//    pthread_exit(NULL);
//}

void* thread_body(void* arg) {
    thread_arg_t *info = (thread_arg_t*) arg;
    pthread_t child, shooter;
    int exit_flag = 0;
    pthread_mutex_t exit_mutex;
    int health_save = INIT_HEALTH; 
    int num = 0;

    printf("%c generation #%d starting...\n", info->name, info->gen);

    pthread_mutex_init(&exit_mutex, NULL);
    //shooter_arg_t *shooter_arg = malloc(sizeof(shoot_arg_t));
    //shooter_arg->name = info->name;
    //shooter_arg->target = info->target;
    //shooter_arg->gen = info->gen;
    //shooter_arg->exit_flag = &exit_flag;
    //shooter_arg->target_mutex = info->target_mutex;
    //shooter_arg->target_cond = info->target_cond;
    //shooter_arg->exit_mutex = &exit_mutex;
    shooter_arg_t shooter_arg = {info->name, info->target, info->gen, &exit_flag, info->target_mutex, info->target_cond, info->target_health, &exit_mutex};

    //pthread_create(&shooter, NULL, shooter_thread, (void*)(&shooter_arg));

    while(1) {
        // Now I'm the health calculation thread
        pthread_mutex_lock(info->my_mutex);
        if(*info->my_health != health_save) {
            health_save = *info->my_health;
            printf("%c generation #%d is shot: [%d/%d]...\n", info->name, info->gen, *info->my_health, INIT_HEALTH);
            if(*info->my_health == 0) {
                //printf("%c generation #%d: Now I'm dead, notifying shooter to exit\n", info->name, info->gen);
                pthread_mutex_unlock(info->my_mutex);
                break;
            }
        }
        pthread_mutex_unlock(info->my_mutex);
        //Shooting other
        pthread_mutex_lock(info->target_mutex);
        if(*info->target_health) {
            printf("%c generation #%d shooting %c #%d\n", info->name, info->gen, info->target, num);
            num++;
            (*info->target_health)--;
        }
        pthread_mutex_unlock(info->target_mutex);
    }

    printf("%c generation #%d is dead\n", info->name, info->gen);

    // Now if generation is smaller than GENERATION_CT, make a child to revenge
    pthread_mutex_lock(info->my_mutex);
    if(info->gen < GENERATION_CT) {
        printf("%c generation #%d makes a child\n", info->name, info->gen);
        *info->my_health = INIT_HEALTH;
        info->gen++;
        pthread_create(&child, &attr, thread_body, (void*)info);
        pthread_mutex_unlock(info->my_mutex);
        pthread_exit(NULL);
    }
    pthread_mutex_unlock(info->my_mutex);
    printf("%c generation #%d is dead, %c wins!\n", info->name, info->gen, info->target);
    printf("%c generation #%d exiting the entire process\n", info->name, info->gen);
    exit(0);

}

int main() {
    pthread_t threads[2];

    printf("Main thread starting...\n");

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&threads[0], &attr, thread_body, (void*)(&A_arg));
    pthread_create(&threads[1], &attr, thread_body, (void*)(&B_arg));

    //pthread_join(threads[0], NULL);
    //pthread_join(threads[1], NULL);

    printf("Main thread exiting...\n");
    pthread_exit(NULL);
    //return 0;
}
