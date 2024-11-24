# paths
MODULES = ../module
INCLUDE = ../include
FILES = ../Input

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall -no-pie -fPIE -g -I$(INCLUDE)

# Αρχεία .o
INCL = $(INCLUDE)/includes.h 
OBJS = program.o $(MODULES)/my_rand.o

# the executable program
EXEC = program

# parameters
ARGS = 5 3

$(EXEC) $(EXEC1): $(OBJS) $(INCL)
	gcc -lpthread -pthread $(OBJS) -o $(EXEC)

clean:
	rm -f $(OBJS) $(EXEC) $(OBJS1) $(EXEC1)


run: $(EXEC)
	./$(EXEC) $(ARGS)

gdb: $(OBJS)
	g++ $(OBJS) -g3 -o $(EXEC)
	gdb $(EXEC)