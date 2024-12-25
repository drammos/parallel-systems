# paths
MODULES = modules
INCLUDE = include
FILES = ../Input

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall -no-pie -fPIE -g -I$(INCLUDE) -fopenmp
LDFLAGS = -fopenmp -lrt

# Αρχεία .o
INCL = $(INCLUDE)/includes.h $(INCLUDE)/Serial.h $(INCLUDE)/Parallel.h
OBJS = program.o $(MODULES)/Serial.o $(MODULES)/Parallel.o

# the executable program
EXEC = program

# parameters
ARGS = 100000 serial column 10

$(EXEC) $(EXEC1): $(OBJS) $(INCL)
	gcc -lpthread -pthread $(OBJS) -o $(EXEC) $(LDFLAGS) -lrt -lm

clean:
	rm -f $(OBJS) $(EXEC) $(OBJS1) $(EXEC1)


run: $(EXEC)
	./$(EXEC) $(ARGS)

gdb: $(OBJS)
	g++ $(OBJS) -g3 -o $(EXEC)
	gdb $(EXEC)