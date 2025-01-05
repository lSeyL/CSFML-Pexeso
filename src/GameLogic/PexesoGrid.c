#include "PexesoGrid.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

PexesoGrid* pexeso_grid_create(int rows, int columns, sfVector2f startPosition, sfVector2f pexesoSize) {
    PexesoGrid* grid = (PexesoGrid*)malloc(sizeof(PexesoGrid));
    if (!grid) return NULL;
    grid->rows = rows;
    grid->columns = columns;
    grid->startPosition = startPosition;
    grid->pexesoSize = pexesoSize;
    grid->pexesoObjects = (Pexeso**)malloc(sizeof(Pexeso*) * rows * columns);
    if (!grid->pexesoObjects) {
        free(grid);
        return NULL;
    }
    pexeso_grid_generate(grid);
    return grid;
}

void pexeso_grid_destroy(PexesoGrid* grid) {
    if (grid) {
        for (int i = 0; i < grid->rows * grid->columns; ++i) {
            if (grid->pexesoObjects[i]) {
                destroy(grid->pexesoObjects[i]);
            }
        }
        free(grid->pexesoObjects);
        free(grid);
    }
}

void pexeso_grid_generate(PexesoGrid* grid) {
    PexesoPair* pairs = pexeso_pairs_generate(grid->rows, grid->columns);
    if (!pairs) {
        printf("Failed to generate pairs\n");
        return;
    }

    for (int row = 0; row < grid->rows; ++row) {
        for (int col = 0; col < grid->columns; ++col) {
            int index = row * grid->columns + col;
            sfVector2f position = {grid->startPosition.x + col * (grid->pexesoSize.x + 10),
                                   grid->startPosition.y + row * (grid->pexesoSize.y + 10)};
             Pexeso* pex = pexesoCreate(&position, &grid->pexesoSize, sfBlack, pairs[index].color, pairs[index].label);
             setID(pex, index);
             grid->pexesoObjects[index] = pex;
             //printf("Card index: %d\n", getID(pex));
        }
    }

    free(pairs);
}

void pexeso_grid_shuffle(PexesoGrid* grid) {
    int count = grid->rows * grid->columns;
    srand(time(NULL));
    for (int i = count - 1; i > 0; --i) {
        int j = rand() % (i + 1);

        Pexeso* temp = grid->pexesoObjects[i];
        grid->pexesoObjects[i] = grid->pexesoObjects[j];
        grid->pexesoObjects[j] = temp;
    }
}

void pexeso_grid_draw(PexesoGrid* grid, sfRenderWindow* window) {
    for (int i = 0; i < grid->rows * grid->columns; ++i) {
        pexesoDraw(grid->pexesoObjects[i], window);
    }
}

void pexeso_grid_handle_click(PexesoGrid* grid, const sfEvent* event) {
    if (!grid || !event) return;
    if (event->type == sfEvtMouseButtonPressed) {
        for (int i = 0; i < grid->rows * grid->columns; ++i) {
            handleClickOnCard(grid->pexesoObjects[i], event);
        }
    }
}