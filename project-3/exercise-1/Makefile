# Compile options.
CFLAGS = -g -Wall


# Object files
OBJS = program.o


EXEC = program

ARGS = 1000 64

$(EXEC): $(OBJS)
	mpicc $(OBJS) -o $(EXEC)

#Build object files
%.o: %.c
	mpicc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	mpiexec -n 4 ./$(EXEC) $(ARGS)

gdb: $(OBJS)
	mpicc $(OBJS) -g3 -o $(EXEC)
	gdb $(EXEC)
