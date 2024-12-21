
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>

#include "Serial.h"
#include "Grid.h"

#include <stdint.h>



void* serial_algorithm(int states, int dimension)
{
    // time()
    Cell** grid = create_grid(dimension);

    // Next states
    //





    
    printf("THE GRID\n");
    for(int i = 0; i < dimension; i++){
        for(int j = 0; j < dimension; j++){
            if(grid[i][j].state == LIVE){
                printf("o");
            }
            else{
                printf("b");
            }
        }
        printf("\n");
    }
    printf("\n");
    
}