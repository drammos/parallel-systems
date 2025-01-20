MODULES = modules
INCLUDE = include
FILES = files

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -g -Wall -I$(INCLUDE)

# Αρχεία .o
INCL = $(INCLUDE)/includes.h $(INCLUDE)/MPI.h $(INCLUDE)/Grid.h

# Object files
OBJS = program.o $(MODULES)/MPI.o $(MODULES)/Grid.o


# Executable program
EXEC = program

# Parameters for running the program
ARGS = 100 64

$(EXEC): $(OBJS)
	mpicc $(OBJS) -o $(EXEC)

# Rule to build object files
%.o: %.c $(INCL)
	mpicc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	mpiexec -n 4 ./$(EXEC) $(ARGS)

gdb: $(OBJS)
	mpicc $(OBJS) -g3 -o $(EXEC)
	gdb $(EXEC)
