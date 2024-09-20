/*
 * Synchronization primitives.
 * See synch.h for specifications of the functions.
 */

#include <types.h>
#include <lib.h>
#include <synch.h>
#include <thread.h>
#include <curthread.h>
#include <machine/spl.h>

////////////////////////////////////////////////////////////
//
// Semaphore.

struct semaphore *
sem_create(const char *namearg, int initial_count)
{
	struct semaphore *sem;

	sem = kmalloc(sizeof(struct semaphore));
	if (sem == NULL) {
		return NULL;
	}

	sem->name = kstrdup(namearg);
	if (sem->name == NULL) {
		kfree(sem);
		return NULL;
	}

	sem->count = initial_count;
	return sem;
}

void
sem_destroy(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);

	spl = splhigh();
	assert(thread_hassleepers(sem)==0);
	splx(spl);

	/*
	 * Note: while someone could theoretically start sleeping on
	 * the semaphore after the above test but before we free it,
	 * if they're going to do that, they can just as easily wait
	 * a bit and start sleeping on the semaphore after it's been
	 * freed. Consequently, there's not a whole lot of point in 
	 * including the kfrees in the splhigh block, so we don't.
	 */

	kfree(sem->name);
	kfree(sem);
}

void 
P(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);

	/*
	 * May not block in an interrupt handler.
	 *
	 * For robustness, always check, even if we can actually
	 * complete the P without blocking.
	 */
	assert(in_interrupt==0);

	spl = splhigh();
	while (sem->count==0) {
		thread_sleep(sem);
	}
	assert(sem->count>0);
	sem->count--;
	splx(spl);
}

void
V(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);
	spl = splhigh();
	sem->count++;
	assert(sem->count>0);
	thread_wakeup(sem);
	splx(spl);
}

////////////////////////////////////////////////////////////
//
// Lock.

struct lock *
lock_create(const char *name)
{
	struct lock *lock;

	lock = kmalloc(sizeof(struct lock));
	if (lock == NULL) {
		return NULL;
	}

	lock->name = kstrdup(name);
	if (lock->name == NULL) {
		kfree(lock);
		return NULL;
	}
	
	// add stuff here as needed
	
	return lock;
}

void
lock_destroy(struct lock *lock)
{
	assert(lock != NULL);

	// add stuff here as needed
	
	kfree(lock->name);
	kfree(lock);
}

void
lock_acquire(struct lock *lock)
{
	// Write this
	int spl;
	// Turn off interrupts
	spl = splhigh();
	
	//Check deadlock
	if(lock_do_i_hold(lock)) {
		panic("lock %s at %p: Deadlock.\n", lock->name, lock);
	}
	
	//Wait until lock becomes free
	while(lock->holder != NULL) {
		/*Sleep thread til unlocked*/
		thread_sleep(lock);
	}
	//Thread is now holding lock
	//set lock holder to current thread
	lock->holder = curthread;
	
	splx(spl);
}

void
lock_release(struct lock *lock)
{
	// Write this
	int spl = splhigh();
	
	//Release the lock
	lock->holder = NULL;
	//Wake thread back up
	thread_wakeup(lock);
	
	//Restore spl level
	splx(spl);
}

int
lock_do_i_hold(struct lock *lock)
{
	// Write this
	int spl, same;
	//Assert input argument lock
	assert(lock !=NULL);
	
	//Turn off interrupts
	spl = splhigh();
	
	//Check if this thread is the lock holder
	if(lock->holder == curthread) {
		same = 1; /* True */
	}
	else {
		same = 0; /* False */
	}
	
	splx(spl);
	return same;
}

////////////////////////////////////////////////////////////
//
// CV


struct cv *
cv_create(const char *name)
{
	struct cv *cv;

	cv = kmalloc(sizeof(struct cv));
	if (cv == NULL) {
		return NULL;
	}

	cv->name = kstrdup(name);
	if (cv->name==NULL) {
		kfree(cv);
		return NULL;
	}
	
	// add stuff here as needed
	
	return cv;
}

void
cv_destroy(struct cv *cv)
{
	assert(cv != NULL);

	// add stuff here as needed
	
	kfree(cv->name);
	kfree(cv);
}

void
cv_wait(struct cv *cv, struct lock *lock)
{
	// Write this
	//Disable interrupts
	int spl = splhigh();
	
	//Release the lock
	lock_release(lock);
	
	//Go to sleep
	thread_sleep(lock);
	
	//After wakeup, acquire the lock
	lock_acquire(lock);
	
	//Restore interrupts
	splx(spl);
}

void
cv_signal(struct cv *cv, struct lock *lock)
{
	// Write this
	//Disable interrupts
	int spl = splhigh();
	
	//Wake up
	thread_wakeup(cv);

	//Restore interrupts
	splx(spl);
}

void
cv_broadcast(struct cv *cv, struct lock *lock)
{
	// Write this
	//Disable interrupts
	int spl = splhigh();
	
	//Wake up and restore
	lock_release(lock);
	thread_wakeup(cv);	

	//Restore interrupts
	splx(spl);
}
