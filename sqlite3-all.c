/******************************************************************************
** This file is an amalgamation of many separate C source files from SQLite
** version 3.12.2.  By combining all the individual C code files into this 
** single large file, the entire code can be compiled as a single translation
** unit.  This allows many compilers to do optimizations that would not be
** possible if the files were compiled separately.  Performance improvements
** of 5% or more are commonly seen when SQLite is compiled as a single
** translation unit.
**
** This file is all you need to compile SQLite.  To use SQLite in other
** programs, you need this file and the "sqlite3.h" header file that defines
** the programming interface to the SQLite library.  (If you do not have 
** the "sqlite3.h" header file at hand, you will find a copy embedded within
** the text of this file.  Search for "Begin file sqlite3.h" to find the start
** of the embedded sqlite3.h header file.) Additional code files may be needed
** if you want a wrapper to interface SQLite with your choice of programming
** language. The code for the "sqlite3" command-line shell is also in a
** separate file. This file contains only code for the core SQLite library.
*/

#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
#include "sqlite-config.h"


#define SQLITE_AMALGAMATION 1
#ifndef SQLITE_PRIVATE
# define SQLITE_PRIVATE static
#endif

#include "sqlite3-1.c"
#include "sqlite3-2.c"
#include "sqlite3-3.c"
#include "sqlite3-4.c"
#include "sqlite3-5.c"
#include "sqlite3-6.c"
#include "sqlite3-7.c"

struct mAllocHeader {
	int			size;
	int			allocationType;
}; 

enum {
	VMALLOC,
	KMALLOC
};
#define MALLOC_HEADER_SIZE sizeof(struct mAllocHeader)
void* kernelMemMalloc(int size) {
	printk("CALLED MALLOC with size: %d\n", size);
	void *ret = NULL;
	struct mAllocHeader mhdr;
	mhdr.size = size;
	mhdr.allocationType = KMALLOC;
	ret = kmalloc(size, GFP_KERNEL);
	printk("Malloc: %p\n", ret);
	if (ret) goto finish;
	
	ret = vmalloc((unsigned long)size);
	mhdr.allocationType = VMALLOC;
 	printk("VMAlloc: %p\n", ret);

	finish:
	if (ret == NULL) return NULL;
	
	memcpy(ret, &mhdr, MALLOC_HEADER_SIZE);
	ret = ret + MALLOC_HEADER_SIZE;
	printk("Returning: %p\n", ret);
	return ret;
}
void kernelMemFree(void *p) {
	printk("Freeing: %p\n", p);
	struct mAllocHeader mhdr;
	memcpy(&mhdr, p - MALLOC_HEADER_SIZE, MALLOC_HEADER_SIZE);
	if (mhdr.allocationType == KMALLOC) {
			kfree(p - MALLOC_HEADER_SIZE);
	} else if (mhdr.allocationType == VMALLOC) {
		vfree(p);
	}
}

void* kernelMemRealloc(void *oldAlloc, int newsize) {
	struct mAllocHeader mhdr;
	void *newalloc = kernelMemMalloc(newsize);

	memcpy(&mhdr, oldAlloc - MALLOC_HEADER_SIZE, MALLOC_HEADER_SIZE);	
	printk("Reallocing(%lld -> %d): %p\n", mhdr.size, newsize, oldAlloc);

	if (!newalloc) { return NULL; }
	// It's okay, because SQLite actually gives us the beginning of the data sections on both
	memcpy(newalloc, oldAlloc, MIN(mhdr.size, newsize));
	
	kernelMemFree(oldAlloc);
	return newalloc;
}

int kernelMemSize(void *p) {

	struct mAllocHeader mhdr;
	memcpy(&mhdr, p - MALLOC_HEADER_SIZE, MALLOC_HEADER_SIZE);
	printk("Memsize, returning: %p, %lld\n", p, mhdr.size);

	return mhdr.size;
}

int kernelMemRoundup(int size) {
	return size * 8;
	int r = size % 8;
	if (r == 0) return size;
	return (8 - r) + size;
}
int kernelMemInit(void *data) {
	printk("CALLED MemINIT\n");

	return SQLITE_OK;
}
int kernelMemShutdown(void *data) {
	return SQLITE_OK;
}
/////////
#define STATIC_VFS_COUNT SQLITE_MUTEX_STATIC_VFS3

struct recursive_spinlock {
	int				 permanent;
	spinlock_t lock;
  pid_t			 pid;
	atomic_t	 count;
};

struct recursive_spinlock static_spinlocks[STATIC_VFS_COUNT];


void initialize_spinlock(struct recursive_spinlock *lock) {
	atomic_set(&lock->count, 0);
	lock->pid = -1;
	spin_lock_init(&lock->lock);
	lock->permanent = 0;
}

void setupSpinlocks(void) {
	int i;
	for (i = 0; i < SQLITE_MUTEX_STATIC_VFS3; i++) {
		initialize_spinlock(&static_spinlocks[i]);
		static_spinlocks[i].permanent = 1;
	}
}

sqlite3_mutex* kernelMutexAlloc(int locknum) {
	struct recursive_spinlock *lock;
	if (locknum == 1 || locknum == 2) {
		lock = kmalloc(sizeof(struct recursive_spinlock), GFP_KERNEL);
		if (!lock) return SQLITE_NOMEM;
		initialize_spinlock(lock);
		return lock;
	} else {
		lock = &static_spinlocks[locknum - 1];
		return lock;
	}
	return NULL;
}
void kernelMutexFree(sqlite3_mutex* m) {
	struct recursive_spinlock *lock = m;
	if (lock->permanent == 1) return;
	kfree(m);
	return;
}
void kernelMutexEnter(sqlite3_mutex* m) {
	struct recursive_spinlock *lock = m;
	int rc = spin_trylock(&lock->lock);
	if (rc == 0) {
		atomic_inc(&lock->count);
		lock->pid = current->pid;
	} else if (current->pid == lock->pid) { 
		atomic_inc(&lock->count);
	} else {
		spin_lock(&lock->lock);
		atomic_inc(&lock->count);
		lock->pid = current->pid;
	}
}
int kernelMutexTry(sqlite3_mutex* m) {
	struct recursive_spinlock *lock = m;
	int rc = spin_trylock(&lock->lock);
	if (rc == 0) { 
		atomic_inc(&lock->count);
		lock->pid = current->pid;
		return SQLITE_OK;
	} else if (current->pid == lock->pid) {
		atomic_inc(&lock->count);
		return SQLITE_OK;
	} 
	return SQLITE_BUSY;
}
void kernelMutexLeave(sqlite3_mutex* m) {
	struct recursive_spinlock *lock = m;
	if (atomic_dec_and_test(&lock->count)) {
		lock->pid = NULL;
		spin_unlock(&lock->lock);
	}
}

int kernelMutexInit(void) {
	return 0;
}

int kernelMutexShutdown(void) {
	return 0;
}

int sqlite3_os_init(void) {
	return 0;
}

void setup_sqlite3(void) {
	printk("Beginning SQLite Initialization process\n");
	static const sqlite3_mutex_methods kernel_mutex_methods = {
		kernelMutexInit,
		kernelMutexShutdown,
		kernelMutexAlloc,
		kernelMutexFree,
		kernelMutexEnter,
		kernelMutexTry,
		kernelMutexLeave,
		0,
		0,
	};
	//setupSpinlocks();
	
	//sqlite3_config(SQLITE_CONFIG_MUTEX, &kernel_mutex_methods);
	static const sqlite3_mem_methods kernel_mem_methods = {
		kernelMemMalloc,         /* Memory allocation function */
		kernelMemFree,          /* Free a prior allocation */
		kernelMemRealloc,  /* Resize an allocation */
		kernelMemSize,          /* Return the size of an allocation */
		kernelMemRoundup,          /* Round up request size to allocation size */
		kernelMemInit,           /* Initialize the memory allocator */
		kernelMemShutdown, /* Deinitialize the memory allocator */
		0,                /* Argument to xInit() and xShutdown() */
	};
	sqlite3_config(SQLITE_CONFIG_MALLOC, &kernel_mem_methods);
}
int sqlite3_os_end(void) {
	
}