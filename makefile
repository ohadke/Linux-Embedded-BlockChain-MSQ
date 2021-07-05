	
	# Simple makefile create executable(.out) from every .c file
CC = gcc
LIBS = -lpthread -lz -lrt

# SRC will hold an array of all the .c files
SRCS := $(subst ./,,$(shell find . -name "*.c"))

# OBJS will hold an array of the corresponding .out to the .c files
OBJS := launcher.out miner.out server.out

# all will be the default TARGET and will create all the object files
all: $(OBJS)

libBlockChain.so:   BlockChain.h BlockChain.c
				gcc -c BlockChain.c -o BlockChain.out -fPIC
				gcc -shared BlockChain.out -o libBlockChain.so  -fPIC

# This rule will be executed for every .out file present in the target "all"
%.out: %.c libBlockChain.so
	$(CC) BlockChain.out  $^ -o $@ $(LIBS) -L libBlockChain.so  -fPIC

# Easy way to remove all binaries
clean:
	rm -f *.out *.so 