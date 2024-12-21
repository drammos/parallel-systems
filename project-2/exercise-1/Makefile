# paths
MODULES = modules
INCLUDE = include
FILES = files

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall -no-pie -fPIE -g -I$(INCLUDE)

# Αρχεία .o
INCL = $(INCLUDE)/includes.h $(INCLUDE)/Serial.h $(INCLUDE)/Grid.h
OBJS = program.o $(MODULES)/Serial.o $(MODULES)/Grid.o

# the executable program
EXEC = program

# parameters
ARGS = 100 10 serial 10

$(EXEC) $(EXEC1): $(OBJS) $(INCL)
	gcc -lpthread -pthread $(OBJS) -o $(EXEC) -lrt

clean:
	rm -f $(OBJS) $(EXEC) $(OBJS1) $(EXEC1)


run: $(EXEC)
	./$(EXEC) $(ARGS)

gdb: $(OBJS)
	g++ $(OBJS) -g3 -o $(EXEC)
	gdb $(EXEC)