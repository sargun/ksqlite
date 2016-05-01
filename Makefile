obj-m := ksqlite.o
ksqlite-objs := sqlite3-all.o kmodesqlite.o	
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	rm -f *.mod.c *.ko *.o

