.PHONY: all clean

all: simavr

CFLAGS=$(shell pkg-config --cflags simavr)

clean:
	rm -f *.o simavr

simavr: simavr.o uart_pty.o
	gcc -o $@ $^ $(shell pkg-config --libs simavr) -lpthread -lutil

simavr.o: simavr.c uart_pty.h

uart_pty.o: uart_pty.c uart_pty.h
