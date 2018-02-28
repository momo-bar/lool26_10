/*
 * serial.c
 *
 *  Created on: 2013-08-19
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>

#include "statistics.h"
#include "multilock.h"

void * serial_worker(void * data) {
    unsigned long i, j;
    struct experiment * e = data;

    for (i = 0; i < e->outer; i++) {
        for (j = 0; j < e->inner; j++) {
            unsigned long idx = (i * e->inner) + j;
            statistics_add_sample(e->data, (double) idx);
        }
    }
    return NULL;
}

void serial_initialization(struct experiment * e) {
    e->data = make_statistics();
    e->lock = NULL;
}

void serial_destroy(struct experiment * e) {
    statistics_copy(e->stats, e->data);
    free_statistics(e->data);
    e->data = NULL;
}
