//
// Created by seil0 on 3. 1. 2025.
//

#ifndef PEXESO_GAME_H
#define PEXESO_GAME_H
#include <SFML/Graphics.h>
#include <stdbool.h>
#include "PexesoGrid.h"

typedef struct {
    PexesoGrid* grid;
    int rowSize;
    int colSize;
    bool isRunning;
} Game;


Game* game_create();
void game_destroy(Game* game);
void game_start(Game* game, int rows, int cols, sfRenderWindow* window);
void game_handle_event(Game* game, const sfEvent* event);
void game_draw(Game* game, sfRenderWindow* window);
#endif //PEXESO_GAME_H
