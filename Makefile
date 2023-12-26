TARGET=gen
CPPFLAGS=
CFLAGS=-O0 -g -Wall -D_GNU_SOURCE
LDLIBS=-lm
CC=gcc

SOURCES=generators.c markov_chain_RNG.c main_biased_gen.c
OBJECTS=$(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o *~ core $(TARGET)

.PHONY: clean
