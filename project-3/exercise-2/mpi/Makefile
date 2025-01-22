# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -g -Wall

# Object files
OBJS = program.o


EXEC = program

ARGS = 16000

$(EXEC): $(OBJS)
	mpicc $(OBJS) -o $(EXEC)

#Build object files
%.o: %.c 
	mpicc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	mpiexec -f ../../machines -n 4 ./$(EXEC) $(ARGS)

gdb: $(OBJS)
	mpicc $(OBJS) -g3 -o $(EXEC)
	gdb $(EXEC)