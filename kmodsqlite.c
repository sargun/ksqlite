#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/module.h>
#include "sqlite-config.h"
#include "sqlite3.h"


static int __init ksqlite_init(void) {
  return 0;
}
static void __exit ksqlite_exit(void) {
}

module_init(ksqlite_init)
module_exit(ksqlite_exit)
MODULE_LICENSE("GPL and additional rights");
