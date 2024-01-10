PROGRAM=logfind
CFLAGS=-Wall -g -O3
OBJECTS=src/*.c
CC=gcc
LDLIBS=-L/usr/local/lib -lgsl -lgslcblas -lm

all:
	make $(PROGRAM)

$(PROGRAM): $(OBJECTS)

clear:
	rm -f $(PROGRAM)
