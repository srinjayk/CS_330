#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<common.h>

/* XXX NOTE XXX  
       Do not declare any static/global variables. Answers deviating from this 
       requirement will not be graded.
*/
void init_rwlock(rwlock_t *lock)
{
   /*Your code for lock initialization*/
	// init_rwlock(&lock->read_lock);
	// init_rwlock(&lock->write_lock);
	// lock->value = 0;
	lock->value = 0x1000000000000;
}

void write_lock(rwlock_t *lock)
{
   /*Your code to acquire write lock*/
	// lock(&rl->write_lock);
	while(1){
		if (atomic_add(&(lock->value),-0x1000000000000) != 0)
		{
			atomic_add(&(lock->value),+0x1000000000000);
			sched_yield();
		}
		else{
			break;
		}
	}
	// lock->value = 0x0;
	// while(atomic_add(&(lock->value),-0x1000000000000)){
	// 	// atomic_add(lock,0x0001000000000000);
	// 	// printf("In write lock loop\n");
	// }
}

void write_unlock(rwlock_t *lock)
{
   /*Your code to release the write lock*/
	// unlock(&rl->write_lock);
	// lock->value = lock->value - 1;
	// if(lock->value == 0x)
	while(1){
		if (atomic_add(&(lock->value),0) == 0)
		{
			atomic_add(&(lock->value),+0x1000000000000);
			sched_yield();
			break;
		}
		else{
			continue;
		}
	}
}


void read_lock(rwlock_t *lock)
{
   /*Your code to acquire read lock*/
	// lock(&rt->read_lock);
	// rt->value++;
	// if(rt->value == 1){
	// 	lock(rt->write_lock);
	// }
	// unlock(&rt->read_lock);
	// if(lock->value == 0x0001000000000000){
	// 	return;
	// }
	// lock->value = lock->value - 1;
	while(1){
		if (atomic_add(&(lock->value),-1) < 0)
		{
			atomic_add(&(lock->value),+1);
			sched_yield();
		}
		else{
			break;
		}
	}
	// {
		// atomic_add(&(lock->value),-1);
		// if(lock->value == 0x0-1){
		// 	write_lock(lock);
		// }
	// }
	// if(lock->value == 0x0){
		// if(lock->value == 0xffffffffffffffff){
		// 	lock->value = lock->value & 0xfffeffffffffffff;
		// }
	// }
}

void read_unlock(rwlock_t *lock)
{
   /*Your code to release the read lock*/
	// lock(&rt->read_lock);
	// rt->value--;
	// if(rt->value == 0){
	// 	unlock(rt->write_lock);
	// }
	// unlock(&rt->read_lock);
	atomic_add(&(lock->value),+1);
	sched_yield();
	// // if(lock->value == 0xfffeffffffffffff + 1){
	// // 	lock->value = 0x0001000000000000;
	// // }
	// if(lock->value == 0x0){
	// 	lock->value = 0x0001000000000000;
	// }
	// lock->value = 0x0001000000000000;
}
