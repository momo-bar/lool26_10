/*
 * semrelay.c
 *
 *  Created on: 2013-08-19
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>

#include "semrelay.h"
#include "statistics.h"
#include "multilock.h"
#include "utils.h"

void *semrelay_worker(void * data) {
    unsigned long i, j;
    struct experiment * exp_data = data;

    // TODO: protection de la boucle interne par un semrelay
    for (i = 0; i < exp_data->outer; i++) {
        sem_wait(&((sem_t*)exp_data->lock)[exp_data->rank]);
        for (j = 0; j < exp_data->inner; j++) {
            /* En mode instable, le thread au rang 0 peut être tué aléatoirement */
            if (j == 0 && exp_data->unstable && exp_data->rank == 0) {
                if (rand() / RAND_MAX < 0.1)
                    pthread_exit(NULL);
            }
            unsigned long idx = (i * exp_data->inner) + j;
            statistics_add_sample(exp_data->data, (double) idx);
        }
        
        sem_post(&((sem_t*)exp_data->lock)[(exp_data->rank + 1) % exp_data->nr_thread]);
    }
    return NULL;
}


void semrelay_initialization(struct experiment * exp_data) {
	exp_data->data = make_statistics();

	// TODO: allocation d'un tableau de sémaphores sem_t dans exp_data->lock
    exp_data->lock = (sem_t*)malloc(sizeof(sem_t) * exp_data->nr_thread);
    
    // TODO: initialisation des sémaphores
    
    for(int i = 0; i < exp_data->nr_thread; i++){
        sem_init(&(((sem_t*)exp_data->lock)[i]),0,0);
    }

    sem_post(&(((sem_t*)exp_data->lock)[0]));
}

void semrelay_destroy(struct experiment * exp_data) {
    int i;

    // copie finale dans exp_data->stats
    statistics_copy(exp_data->stats, exp_data->data);
    free(exp_data->data);

    // TODO: destruction du verrou
    for(i = 0; i < exp_data->nr_thread; i++){
        sem_destroy(&(((sem_t*)exp_data->lock)[i]));
    }
    // TODO: liberation de la memoire du verrou
    free(exp_data->lock);
}

