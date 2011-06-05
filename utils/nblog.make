#
# This makefile makes the php log plugin and install it, as I 
# did not know how to di this in auto make, yet.
#   
all: nblog.so

CFLAGS=-O2 -fpic -DCOMPILE_DL=1 $(shell php-config --includes)
LDFLAGS=-shared
OBJS=nblog_php.o

nblog_php.o: nblog_php.c

nblog.so: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(LIBS) $(OBJS)

install: nblog.so
	cp nblog.so $(shell php-config --extension-dir)

clean:
	rm -f nblog_php.o nblog.so
