TARGET=gen
TARGET2=test
CPPFLAGS=
CFLAGS=-O0 -g -Wall -D_GNU_SOURCE
LDLIBS=-lm
CC=gcc

SOURCES=generators.c markov_chain_RNG.c main_biased_gen.c
OBJECTS=$(SOURCES:.c=.o)

SOURCES2=generators.c markov_chain_RNG.c main.c
OBJECTS2=$(SOURCES2:.c=.o)

all: $(TARGET) $(TARGET2)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDLIBS)

$(TARGET2): $(OBJECTS2)
	$(CC) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o *~ core $(TARGET) $(TARGET2)

.PHONY: clean
