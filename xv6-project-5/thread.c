#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

int thread_create(void *(*function)(void *), int priority, void *arg, void *stack)
{
	int tid;

	tid = do_thread_create(priority,*function,arg,stack);

	if(!tid) {
		(*function)(arg);
		return 0;
	}
	else {
		yield();
		return tid;
	}
}

void thread_exit(void *retval)
{
	do_thread_exit(retval);
}

int thread_join(int tid, void **retval)
{
	return do_thread_join(tid, retval);
}

int gettid(void)
{
	return do_gettid();
}
