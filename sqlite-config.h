#include <linux/compiler.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/ktime.h>

#define SQLITE_OS_OTHER 1
#define SQLITE_CORE 1
#define SQLITE_AMALGAMATION 1


#define SQLITE_OMIT_COMPILEOPTION_DIAGS 1


#define SQLITE_SMALL_STACKS 1
#define SQLITE_THREADSAFE 1

// It could make queries go faster?
#define SQLITE_ENABLE_STAT3 1
#define SQLITE_ENABLE_STAT4 1

#define SQLITE_ENABLE_HIDDEN_COLUMNS 1

#define SQLITE_CASE_SENSITIVE_LIKE 1

// Set into NORMAL locking mode, which shouldn't matter, given we will only use sqlite in memory
#define SQLITE_DEFAULT_LOCKING_MODE 0

#define SQLITE_DISABLE_DIRSYNC 1
#define SQLITE_DISABLE_LFS 1

// No idea what this is
#define SQLITE_ENABLE_8_3_NAMES 1

// TODO: Disable when we're confident
#define SQLITE_ENABLE_API_ARMOR 0


#define SQLITE_ENABLE_COLUMN_METADATA 1
#define SQLITE_ENABLE_DBSTAT_VTAB 1


// JSON is cool, right?
#define SQLITE_ENABLE_JSON1 1

/*
#define SQLITE_ENABLE_MEMORY_MANAGEMENT 0
#define SQLITE_ENABLE_MEMSYS3 0
#define SQLITE_ENABLE_MEMSYS5 0
#define SQLITE_ENABLE_SQLLOG 0
#define SQLITE_ENABLE_UNLOCK_NOTIFY 1
*/

    
/* Now we start trimming features */
#define SQLITE_OMIT_AUTHORIZATION 1

//#define SQLITE_OMIT_DISKIO 1
#define SQLITE_OMIT_DEPRECATED 1
#define SQLITE_OMIT_SHARED_CACHE 1
#define SQLITE_OMIT_TCL_VARIABLE 1 
#define SQLITE_OMIT_WAL 1

#define SQLITE_ZERO_MALLOC 1

#define SQLITE_OMIT_DATETIME_FUNCS 1
#define SQLITE_OMIT_LOCALTIME 1
#define SQLITE_OMIT_FLOATING_POINT 1

#define SQLITE_MUTEX_APPDEF 1
//#define SQLITE_HOMEGROWN_RECURSIVE_MUTEX 1
//#define SQLITE_MUTEX_OMIT 1

#define HAVE_STRCHRNUL 1
#define HAVE_ISNAN 0

#define SQLITE_COVERAGE_TEST 0

#define assert(X)