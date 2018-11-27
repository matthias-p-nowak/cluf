SRCS=$(wildcard *.c)
TSRCS=run_cunit.c $(wildcard test*.c)
SRCS:=$(filter-out $(TSRCS),$(SRCS))
OBJS=$(SRCS:.c=.o)
HDRS=$(wildcard *.h)
MSRC=$(wildcard *main.c)
TOBJS=$(filter-out $(MSRC:.c=.o), $(OBJS)) $(TSRCS:.c=.o)

CFLAGS=-g -H
TARGET=cluf

all: $(TARGET) test

##### rules for target ######
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.h.gch: %.h
	$(CC) $(CFLAGS) -o $@ $^
	
$(OBJS): $(HDRS:.h=.h.gch)

##### rules for testing #####
test: run_test
	./run_test -b

run_cunit.o: running.inc
run_test: $(TOBJS)
	echo MSRC=$(MSRC)
	echo TOBJS=$(TOBJS)
	$(CC) $(CFLAGS) -lcunit -o $@ $^
running.inc: $(wildcard test_*.c)
	grep -h -e CUNIT_SUITE -e CUNIT_TEST $^ >$@

##### general rules #####

clean:
	rm -f *.o $(TARGET) *.gch run_test

git:
	git add *.c *.h makefile
	git commit -a

.PHONY: clean test all git
