/*
 * interblocage.c
 *
 *  Created on: 2013-09-02
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <time.h>

#define WATCHDOG_UDELAY 100000 // 100ms

volatile int x;
volatile int y;

pthread_mutex_t lock_one;
pthread_mutex_t lock_two;
pthread_barrier_t barrier;

pthread_t threads[2];

/*
 * Calcule pour un temps aléatoire
 */
void random_hog()
{
    volatile unsigned long i, x;
    unsigned long count = random() / (1 << 20) + 10000;
    for (i = 0; i < count; i++)
        x++;
}

void * worker_foo(void *data)
{
    while(1) {
        random_hog();

        // TODO 2.1: prendre les locks de manière à provoquer un interblocage
        pthread_mutex_lock(&lock_one);
		pthread_mutex_lock(&lock_two);

        // TODO 2.3: forcer l'interblocage avec la barriere
		pthread_barrier_wait(&barrier);

        x = ++y;
        printf("foo %d\n", x);

        // TODO 2.1: relacher les locks
        //pthread_mutex_lock(&lock_one);
        pthread_mutex_unlock(&lock_two);
		pthread_mutex_unlock(&lock_one);
		

    }
    return NULL;
}

void * worker_bar(void *data)
{
    while(1) {
        random_hog();

        // TODO 2.1: prendre les locks de manière à provoquer un interblocage
        //pthread_mutex_lock(&lock_one);
        pthread_mutex_lock(&lock_two);
        pthread_mutex_lock(&lock_one);
		

        // TODO 2.3: forcer l'interblocage avec la barriere
		pthread_barrier_wait(&barrier);

        x = ++y;
        printf("bar %d\n", x);

        // TODO 2.1: relacher les locks
		pthread_mutex_unlock(&lock_one);
		pthread_mutex_unlock(&lock_two);

    }
    return NULL;
}

/*
 * Initialisation des nombres aléatoires
 */
void init_seed(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    srandom(ts.tv_nsec);
    srandom(time(NULL));
}

/*
 * Fonction de rappel de SIGALRM
 */
static void watchdog(int signr)
{
    (void) signr;

    // TODO 2.2: Si un interblocage est détecté, alors faire appel à exit(0)
    printf("watchdog\n");
}

/*
 * Démarrage du timer
 */
void timer_start() {
    struct itimerval timer;
    struct sigaction action;
    sigset_t set;

    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = WATCHDOG_UDELAY;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = WATCHDOG_UDELAY;

    sigemptyset(&action.sa_mask);
    action.sa_handler = watchdog;
    action.sa_flags = 0;
    sigaction(SIGALRM, &action, NULL);
    setitimer(ITIMER_REAL, &timer, NULL);
}

/*
 * Arrêt du timer
 */
void timer_stop() {
    struct itimerval timer;

    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer, NULL);
}

int main(int argc, char **argv)
{
    init_seed();

    // TODO 2.1: initialiser lock_one et lock_two
	pthread_mutex_init(&lock_one, NULL);
	pthread_mutex_init(&lock_two, NULL);

    // Initialisation de la barriere
    pthread_barrier_init(&barrier, NULL, 2);

    timer_start();

    /*
     * Creation des fils d'execution worker_foo et worker_bar
     * Toutes les variables sont globales, argument NULL
     */
    pthread_create(&threads[0], NULL, worker_foo, NULL);
    pthread_create(&threads[1], NULL, worker_bar, NULL);
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    timer_stop();

    // TODO 2.1: destruction des verrous lock_one et lock_two
	pthread_mutex_destroy(&lock_one);
	pthread_mutex_destroy(&lock_two);

    printf("done\n");
    return 0;
}
