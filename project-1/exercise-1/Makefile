# paths
# MODULES = ../modules
INCLUDE = ../include
FILES = ../Input

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall -no-pie -fPIE -g -I$(INCLUDE)

# Αρχεία .o
INCL = $(INCLUDE)/includes.h 
OBJS = main.o 

# the executable program
EXEC = main

# parameters
ARGS = 1111 1000110

$(EXEC) $(EXEC1): $(OBJS) $(INCL)
	gcc -lpthread -pthread $(OBJS) -o $(EXEC)

clean:
	rm -f $(OBJS) $(EXEC) $(OBJS1) $(EXEC1)


run: $(EXEC)
	./$(EXEC) $(ARGS)

gdb: $(OBJS)
	g++ $(OBJS) -g3 -o $(EXEC)
	gdb $(EXEC)