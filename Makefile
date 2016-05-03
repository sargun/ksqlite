obj-m := ksqlite.o
ksqlite-objs := sqlite3-all.o kmodsqlite.o	
KDIR := /lib/modules/$(shell uname -r)/build
#KDIR := /home/sargun/linux-4.4.8
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	rm -f *.mod.c *.ko *.o

