#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "proc.h"
#include "synch.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int sys_getnice(void) {
	int pid;

	if(argint(0, &pid) < 0)
		return -1;
	return getnice(pid);
}

int sys_setnice(void) {
	int pid,value;

	if(argint(0,&pid) < 0)
		return -1;
	if(argint(1,&value) < 0)
		return -1;
	return setnice(pid,value);
}

int
sys_getpid(void)
{
	return getpid();
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_halt(void)
{
	outw(0xB004, 0x0|0x2000);
	return 0;
}

int sys_thread_create(void){
	int function, priority, arg, stack;

	if(argint(0, &function)<0 || argint(1, &priority)<0 || argint(2, &arg)<0 || argint(3, &stack)<0)
		return -1;

	return thread_create((void *(*)(void *))function, priority, (void *)arg, (void *)stack);
}

int sys_thread_exit(void){
	int retval;

	if(argint(0, &retval)<0)
		return -1;

	thread_exit((void *)retval);
	return 0;
}

int sys_thread_join(void){
	int tid, retval;

	if(argint(0, &tid)<0 || argint(1, &retval)<0)
		return -1;

	return thread_join(tid, (void **)retval);
}

int sys_gettid(void){
	return gettid();
}

int sys_mutex_init(void){
	int mutex;

	if(argint(0, &mutex)<0)
		return -1;

	return mutex_init((struct mutex_t *)mutex);
}

int sys_mutex_lock(void){
	int mutex;

	if(argint(0, &mutex)<0)
		return -1;

	return mutex_lock((struct mutex_t *)mutex);
}

int sys_mutex_unlock(void){
	int mutex;

	if(argint(0, &mutex)<0)
		return -1;

	return mutex_unlock((struct mutex_t *)mutex);
}

int sys_cond_init(void){
	int cond;

	if(argint(0, &cond)<0)
		return -1;

	return cond_init((struct cond_t *)cond);
}

int sys_cond_wait(void){
	int cond, mutex;

	if(argint(0, &cond)<0 || argint(1, &mutex)<0)
		return -1;

	return cond_wait((struct cond_t *)cond, (struct mutex_t *)mutex);
}

int sys_cond_signal(void){
	int cond;

	if(argint(0, &cond)<0)
		return -1;

	return cond_signal((struct cond_t *)cond);
}
