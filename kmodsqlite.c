#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/module.h>
#include "sqlite-config.h"
#include "sqlite3.h"

void setup_sqlite3(void);
void add_vfs(void);

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
    for(i=0; i<argc; i++){
			printk("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	  }
    printk("\n");
    return 0;
}
static int __init ksqlite_init(void) {
	setup_sqlite3();
	sqlite3_initialize();
	sqlite3_config(SQLITE_OPEN_URI, 1);
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	printk("Trying to open database\n");
	add_vfs();
	rc = sqlite3_open_v2(":memory:", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	if( rc ){
		printk("Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
	} else {
		printk("Success\n");
		rc = sqlite3_exec(db, "SELECT sqlite_version();", callback, 0, &zErrMsg);
		if( rc!=SQLITE_OK ){
			printk("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
	}

  return 0;
}
static void __exit ksqlite_exit(void) {
}

module_init(ksqlite_init)
module_exit(ksqlite_exit)
MODULE_LICENSE("GPL and additional rights");
