ALL_SRCS=$(wildcard *.c)
TSRCS=run_cunit.c $(wildcard test*.c)
SRCS=$(filter-out $(TSRCS),$(ALL_SRCS))
OBJS=$(SRCS:.c=.o)
MSRC=$(wildcard *main.c)
TOBJS=$(filter-out $(MSRC:.c=.o), $(OBJS)) $(TSRCS:.c=.o)

CFLAGS=-g -H
TARGET=cluf

all: $(TARGET) test

##### precompiled headers section #####
HDRS=$(wildcard *.h)
GCHS=$(HDRS:.h=.h.gch)
%.h.gch: %.h
	$(CC) $(CFLAGS) -o $@ $^
$(TARGET): $(GCHS)
run_test: $(GCHS)

##### rules for target ######
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)


##### rules for testing #####
test: run_test
	./run_test -b

run_cunit.o: running.inc
run_test: $(TOBJS)
	@echo MSRC=$(MSRC)
	@echo TOBJS=$(TOBJS)
	$(CC) $(CFLAGS) -lcunit -o $@ $(TOBJS)
running.inc: $(wildcard test_*.c)
	grep -h -e CUNIT_SUITE -e CUNIT_TEST $^ >$@

##### general rules #####

clean:
	rm -f *.o $(TARGET) *.gch run_test

git:
	git add *.c *.h makefile
	git commit -a

.PHONY: clean test all git
