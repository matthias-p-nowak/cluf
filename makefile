SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

CFLAGS=-g -H

all: cluf


cluf: $(OBJS)
	@echo new $<
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.h.gch: %.h
	$(CC) $(CFLAGS) -o $@ $^
	
HDRS=$(wildcard *.h)
$(OBJS): $(HDRS:.h=.h.gch)

clean:
	rm -f *.o cluf *.gch

.PHONY: clean test all
