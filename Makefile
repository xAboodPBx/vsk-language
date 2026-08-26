.PHONY: all clean

CC ?= gcc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra

all: vskc

vskc: vsk.c
	$(CC) $(CFLAGS) vsk.c -lm -o vskc

clean:
	rm -f vskc
