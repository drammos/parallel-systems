# Paths
MODULES = modules
INCLUDE = include
FILES = files

# Compile options: -I<dir> tells the compiler to look for header files in that directory
CFLAGS = -Wall -no-pie -fPIE -g -I$(INCLUDE) -fopenmp
LDFLAGS = -fopenmp -lrt

# Header files
INCL = $(INCLUDE)/includes.h $(INCLUDE)/Serial.h $(INCLUDE)/Grid.h $(INCLUDE)/Parallel.h

# Object files
OBJS = program.o $(MODULES)/Serial.o $(MODULES)/Grid.o $(MODULES)/Parallel.o

# Executable program
EXEC = program

# Parameters for running the program
ARGS = 100 10000 parallel 10

# Compilation rule for the final executable
$(EXEC): $(OBJS) $(INCL)
	gcc $(CFLAGS) $(OBJS) -o $(EXEC) $(LDFLAGS)

# Clean up all object files and the executable
clean:
	rm -f $(OBJS) $(EXEC)

# Run the program with arguments
run: $(EXEC)
	./$(EXEC) $(ARGS)

# Debug build with gdb (if you want to debug with gdb, use this target)
gdb: $(OBJS)
	g++ $(OBJS) -g3 -o $(EXEC)
	gdb $(EXEC)
