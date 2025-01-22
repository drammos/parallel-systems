# Compile options: -I<dir> tells the compiler to look for header files in that directory
CFLAGS = -Wall -no-pie -fPIE -g

# Object files
OBJS = program.o

# Executable program
EXEC = program

# Parameters for running the program
ARGS = 4

# Compilation rule for the final executable
$(EXEC): $(OBJS)
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