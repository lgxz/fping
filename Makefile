
include build.include

CFLAGS   = ${BUILD_CFLAGS} -std=gnu99
LDFLAGS  = ${BUILD_LDFLAGS} -lpthread

.PHONY: all clean

all: fping

fping: main.o nedmalloc.o utils.o log.o icmp.o ks_rbtree.o table.o result.o send.o recv.o
	$(call cmd,simple)

clean:
	@rm -rf *.o fping

