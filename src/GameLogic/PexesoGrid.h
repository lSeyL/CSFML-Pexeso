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
    int rows;
    int columns;
    sfVector2f startPosition;
    sfVector2f pexesoSize;
    Pexeso** pexesoObjects;
} PexesoGrid;

PexesoGrid* pexesoGridCreate(int rows, int columns, sfVector2f startPosition, sfVector2f pexesoSize, bool generate);
void pexesoGridDestroy(PexesoGrid* grid);

void pexesoGridGenerate(PexesoGrid* grid);
void pexesoGridShuffle(PexesoGrid* grid);
void pexesoGridDraw(PexesoGrid* grid, sfRenderWindow* window);
void pexesoGridHandleClick(PexesoGrid* grid, const sfEvent* event);
#endif //PEXESO_PEXESOGRID_H
