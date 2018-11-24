SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
HDRS=$(wildcard *.h)
GCHS=$(HDRS:.h=.h.gch)

CFLAGS=-g -H

%.h.gch: %.h
	$(CC) $(CFLAGS) -o $@ $^
	
cluf: $(GCHS) $(OBJS)
	@echo new $<
	$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
	rm -f *.o cluf *.gch

.PHONY: clean test all
