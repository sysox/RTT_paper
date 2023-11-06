TARGET=gen
CPPFLAGS=
CFLAGS=-O3 -g -Wall -D_GNU_SOURCE
LDLIBS=
CC=gcc

SOURCES=biased_gen.c main_biased_gen.c
OBJECTS=$(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o *~ core $(TARGET)

.PHONY: clean
