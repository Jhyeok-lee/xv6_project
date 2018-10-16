#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "spinlock.h"
#include "proc.h"
#include "synch.h"

int mutex_init(struct mutex_t *mutex){

	if(mutex == 0)
		return -1;

	if( mutex->flag == 1)
		return -2;

	mutex->init = 1;
	mutex->flag = 1;

	if( mutex->init != 1)
		return -3;

	return 0;
}

int mutex_lock(struct mutex_t *mutex){

	return do_mutex_lock(mutex);
}

int mutex_unlock(struct mutex_t *mutex){

	return do_mutex_unlock(mutex);;
}

int cond_init(struct cond_t *cond){

	if(cond == 0)
		return -1;

	if(cond->init == 1)
		return -2;

	cond->init = 1;
	cond->flag = 0;

	if( cond->init != 1 || cond->flag != 0)
		return -3;

	return 0;
}

int cond_wait(struct cond_t *cond, struct mutex_t *mutex){

	return do_cond_wait(cond,mutex);
}

int cond_signal(struct cond_t *cond){

	return do_cond_signal(cond);
}
