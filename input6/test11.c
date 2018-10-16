#include "types.h"
#include "stat.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "spinlock.h"
#include "proc.h"
#include "synch.h"
#include "user.h"
#define N 8

void *stack[NTHREAD];
int tid[NTHREAD];
void *retval[NTHREAD];
int buffer[N];
int result[100];
int in=0, out=0;

struct sema_t {
	int v;
	char name[10];
	struct cond_t c;
	struct mutex_t m;
};

struct sema_t empty, full, mutex;

void sema_init(struct sema_t *s, int v, char *name){
	strcpy(s->name, name);
	s->v = v;
	if(cond_init(&s->c)){
		printf(1, "cond init failed\n");
		exit();
	}
	if(mutex_init(&s->m)){
		printf(1, "mutex init failed\n");
		exit();
	}
}

void sema_wait(struct sema_t *s){
	if(mutex_lock(&s->m)){
		printf(1, "mutex lock failed\n");
		exit();
	}

	while(s->v <= 0){
		if(cond_wait(&s->c, &s->m)){
			printf(1, "cond wait failed\n");
			exit();
		}
	}
	s->v--;

	if(mutex_unlock(&s->m)){
		printf(1, "mutex unlock failed\n");
		exit();
	}
}

void sema_signal(struct sema_t *s){
	if(mutex_lock(&s->m)){
		printf(1, "mutex lock failed\n");
		exit();
	}

	s->v++;
	if(cond_signal(&s->c)){
		printf(1, "cond signal failed\n");
		exit();
	}

	if(mutex_unlock(&s->m)){
		printf(1, "mutex unlock failed\n");
		exit();
	}
}

void produce(int data){
	sema_wait(&empty);
	sema_wait(&mutex);
	buffer[in] = data;
	in = (in+1)%N;
	sema_signal(&mutex);
	sema_signal(&full);
}

int consume(void){
	int data;

	sema_wait(&full);
	sema_wait(&mutex);
	data = buffer[out];
	out = (out+1)%N;
	sema_signal(&mutex);
	sema_signal(&empty);

	return data;
}

void *producer(void *arg){
	int i;
	for(i=0;i<100;i++)
		produce((int)i);

	thread_exit(0);
}

void *consumer(void *arg){
	int i;
	for(i=0;i<100;i++)
		result[i] = consume();

	thread_exit(0);
}

int
main(int argc, char **argv)
{
	int i;

	printf(1, "TEST11: ");

	for(i=0;i<NTHREAD;i++)
		stack[i] = malloc(4096);

	sema_init(&mutex, 1, "mutex");
	sema_init(&empty, N, "empty");
	sema_init(&full, 0, "full");

	tid[0] = thread_create(producer, 30, 0, stack[0]);
	if(tid[0] == -1){
		printf(1, "thread create failed\n");
		exit();
	}

	tid[1] = thread_create(consumer, 30, 0, stack[1]);
	if(tid[1] == -1){
		printf(1, "thread create failed\n");
		exit();
	}


	if(thread_join(tid[0], &retval[0]) == -1){
		printf(1, "thread join failed\n");
		exit();
	}

	if(thread_join(tid[1], &retval[1]) == -1){
		printf(1, "thread join failed\n");
		exit();
	}

	for(i=0;i<100;i++){
		if(result[i] != i){
			printf(1, "WRONG\n");
			exit();
		}
	}

	for(i=0;i<NTHREAD;i++)
		free(stack[i]);

	printf(1, "OK\n");

	exit();
}
