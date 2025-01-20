#include "includes.h"
#include "Grid.h"


void parse_rle_header(const char* header_line, int* x, int* y) 
{
    if (sscanf(header_line, " = %d, y = %d", x, y) != 2) {
        fprintf(stderr, "Error: Could not parse RLE header.\n");
        exit(1);
    }
}

Cell** read_file_rle(Cell** grid, int dimension)
{   
    char* file = malloc(sizeof(char));
    file = "files/test.rle";

    FILE *filename = fopen(file, "r");
    char character;
    character = fgetc(filename);


    while (character == '#') {
        char buf[1024];
        fgets(buf, 1024, filename);
        character = fgetc(filename);
    }

    int pattern_x, pattern_y;
   
    if (character == 'x') {
        char buf[1024];
        if (fgets(buf, sizeof(buf), filename) != NULL) {
            parse_rle_header(buf, &pattern_x, &pattern_y);
        } else {
            fprintf(stderr, "Error reading RLE header.\n");
            exit(1);
        }
        character = fgetc(filename);
    }

    int middle_i = dimension/2 - pattern_x/2;
    int middle_j = dimension/2 - pattern_y/2;

    int m = 0;
    int i = middle_i;
    int j = middle_j;

    while (character != EOF)
    {   
        if(character == ' ' || character == '\n' || character == '\t') {
            character = fgetc(filename);
            continue;
        }
    
        if (character == 'b')
        {
            m = (m == 0) ? 1 : m;
            j += m;
            m = 0;
        }
        else if (character == 'o')
        {
            m = (m == 0) ? 1 : m;
            for (int f = 0; f < m; f++) {
                grid[i][j].state = LIVE;
                j++;
            }
            m = 0;
        }
        else if (character == '$')
        {
            m = (m == 0) ? 1 : m;
            i += m;
            j = middle_j;
            m = 0;
        }
        else if (character == '!')
        {
            break;
        }
        else if (isdigit(character)) 
        {    
            m = m * 10 + (character - '0');
        }
        character = fgetc(filename);
    }

    fclose(filename);
    return grid;
}

// Cell** initialize_grid(int dimension)
// {   
//     Cell** grid = malloc(dimension * sizeof(Cell*)); 
//     if (grid == NULL) {
//         perror("Failed to allocate memory for grid");
//         exit(ERROR);
//     }

//     for (int i = 0; i < dimension; i++) {
//         grid[i] = malloc(dimension * sizeof(Cell));
//         if (grid[i] == NULL) {
//             perror("Failed to allocate memory for grid row");
//             exit(ERROR);
//         }
//     }

//     for (int i = 0; i < dimension; i++) {
//         for (int j = 0; j < dimension; j++) {
//             grid[i][j].state = DEAD;
//         }
//     }
//     return grid;
// }
// Initializes a grid with specified dimensions (rows x cols), filling it with DEAD cells
// Cell** initialize_grid(int rows, int cols) {
//     Cell** grid = malloc(rows * sizeof(Cell*));
//     if (grid == NULL) {
//         perror("Failed to allocate memory for grid");
//         // Abort on allocation failure
//     }

//     for (int i = 0; i < rows; i++) {
//         grid[i] = malloc(cols * sizeof(Cell));
//         if (grid[i] == NULL) {
//             perror("Failed to allocate memory for grid row");
//             for (int j = 0; j < i; j++) { // Free already allocated rows
//                 free(grid[j]);
//             }
//             free(grid);
            
//         }
//         // Initialize all cells to DEAD
//         for (int j = 0; j < cols; j++) {
//             grid[i][j].state = DEAD;
//         }
//     }
//     return grid;
// }

// void free_grid(Cell** grid, int rows) {
//     if (grid) {
//         for (int i = 0; i < rows; i++) {
//             if (grid[i]) {
//                 free(grid[i]);
//             }
//         }
//         free(grid);
//     }
// }

// // Reads an RLE file and populates the grid
// Cell** create_grid(int dimension) {
//     Cell** grid = initialize_grid(dimension, dimension);    
//     return read_file_rle(grid, dimension);
// }


// // Prints the grid to the console
// void print_grid(Cell** grid, int dimension) {
//     for (int i = 0; i < dimension; i++) {
//         for (int j = 0; j < dimension; j++) {
//             printf("%c", (grid[i][j].state == LIVE) ? 'O' : '.');
//         }
//         printf("\n");
//     }
// }

// Cell** create_grid(int dimension)
// {
//     Cell** grid = initialize_grid(dimension);    
//     return read_file_rle(grid, dimension);
// }

// void print_grid(Cell** grid, int dimension)
// {
//     printf("THE GRID\n");
//     for(int i = 0; i < dimension; i++){
//         for(int j = 0; j < dimension; j++){
//             if(grid[i][j].state == LIVE){
//                 printf("|");
//             }
//             else{
//                 printf(".");
//             }
//         }
//         printf("\n");
//     }
//     printf("\n");    
// }

// void free_grid(Cell** grid, int dimension)
// {
//     for(int i = 0; i < dimension; i++)
//     {
//         free(grid[i]);
//     }
//     free(grid);
// }