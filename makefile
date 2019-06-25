CFLAGS=-g -Wall -Werror -Wextra
CC=gcc

DEPS=http_measure_priv.h http_measure.h
LIB_OBJ=http_measure.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(LIB_OBJ)
	ar -cvq http_measure.a $(LIB_OBJ)
	$(CC) $(CFLAGS) main.c -o http_measure http_measure.a -lcurl

clean:
	rm -fv *.o
	rm -fv *.a
	rm -fv http_measure

