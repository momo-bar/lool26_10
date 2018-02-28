/*
 * semrelay.h
 *
 *  Created on: 2013-08-19
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#ifndef SEMRELAY_H_
#define SEMRELAY_H_

#include "multilock.h"

void *semrelay_worker(void * data);
void semrelay_initialization(struct experiment * e);
void semrelay_destroy(struct experiment * e);

#endif /* SEMRELAY_H_ */
