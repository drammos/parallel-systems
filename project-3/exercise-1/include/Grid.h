#define ERROR -1

typedef enum {
    DEAD,
    LIVE,
} State;


struct cell{
    State state;
}typedef Cell;

// Cell** create_grid(int dimension);

// Cell** initialize_grid(int rows, int cols);

// void print_grid(Cell** grid, int dimension);

// void free_grid(Cell** grid, int dimension);