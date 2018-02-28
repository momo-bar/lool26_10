/*
 * mutex.c
 *
 *  Created on: 2013-08-19
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "statistics.h"
#include "multilock.h"

#include "utils.h"

void * mutex_worker(void * data) {
    unsigned long i, j, inner;
    struct experiment * exp_data = data;

    // TODO: protection de la boucle for interne par un verrou
	for (i = 0; i < exp_data->outer; i++) {
		pthread_mutex_lock(exp_data->lock);
		pthread_cleanup_push(pthread_mutex_unlock, exp_data->lock);
		
		for (j = 0; j < exp_data->inner; j++) {
			/* En mode instable, le thread au rang 0 peut être tué aléatoirement */
			if (j == 0 && exp_data->unstable && exp_data->rank == 0) {
				if (rand() / RAND_MAX < 0.1)
					pthread_exit(NULL);
			}
			unsigned long idx = (i * exp_data->inner) + j;
			statistics_add_sample(exp_data->data, (double) idx);
		}
		pthread_cleanup_pop(1);
	}
    return NULL;
}

void mutex_initialization(struct experiment * exp_data) {
    exp_data->data = make_statistics();

    // TODO: allocation d'un pthread_mutex_t dans exp_data->lock
    
	exp_data->lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*1);
	// TODO: initialisation du mutex
	
	pthread_mutex_init(exp_data->lock, NULL);
}

void mutex_destroy(struct experiment * exp_data) {
    statistics_copy(exp_data->stats, exp_data->data);
    free(exp_data->data);

	// TODO: destruction du verrou
	
	pthread_mutex_destroy(exp_data->lock);
	
	// TODO: liberation de la memoire du verrou
	
	free(exp_data->lock);
}


