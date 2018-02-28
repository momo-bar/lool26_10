/*
 * mutex.h
 *
 *  Created on: 2013-08-19
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#ifndef MUTEX_H_
#define MUTEX_H_

#include "statistics.h"

void * mutex_worker(void * data);
void mutex_initialization(struct experiment * e);
void mutex_destroy(struct experiment * e);


#endif /* MUTEX_H_ */
