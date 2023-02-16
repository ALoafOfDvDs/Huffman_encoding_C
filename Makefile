CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic -gdwarf-4 -O3



SOURCES  = $(wildcard *.c)
OBJECTS  = $(SOURCES:%.c=%.o)

.PHONY: all clean spotless

all: encode decode

encode: encode.o code.o io.o huffman.o node.o stack.o pq.o 
	$(CC) -o $@ $^

decode: decode.o code.o io.o huffman.o node.o stack.o pq.o 
	$(CC) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o

spotless:
	rm -f encode decode *.o

format:
	clang-format -i -style=file code.c decode.c encode.c huffman.c io.c node.c pq.c stack.c