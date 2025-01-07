//
// Created by seil0 on 3. 1. 2025.
//

#ifndef PEXESO_PEXESOGRID_H
#define PEXESO_PEXESOGRID_H
#include <SFML/Graphics.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#include "PexesoPairs.h"
#include "Pexeso.h"

typedef struct {
    int rows;                   // Number of rows
    int columns;                // Number of columns
    sfVector2f startPosition;   // Starting position for the grid
    sfVector2f pexesoSize;      // Size of each Pexeso card
    Pexeso** pexesoObjects;     // Array of pointers to Pexeso objects
} PexesoGrid;

PexesoGrid* pexeso_grid_create(int rows, int columns, sfVector2f startPosition, sfVector2f pexesoSize, bool generate);
void pexeso_grid_destroy(PexesoGrid* grid);

void pexeso_grid_generate(PexesoGrid* grid);
void pexeso_grid_shuffle(PexesoGrid* grid);
void pexeso_grid_draw(PexesoGrid* grid, sfRenderWindow* window);
void pexeso_grid_handle_click(PexesoGrid* grid, const sfEvent* event);
#endif //PEXESO_PEXESOGRID_H
