//
// Created by seil0 on 3. 1. 2025.
//

#ifndef PEXESO_GAME_H
#define PEXESO_GAME_H
#include <SFML/Graphics.h>
#include <SFML/Network.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "PexesoGrid.h"

typedef struct {
    PexesoGrid* grid;
    int rowSize;
    int colSize;
    sfTcpSocket* socket;
    bool isMultiplayer;
    bool isRunning;
} Game;


Game* game_create();
void game_destroy(Game* game);
void game_start_singleplayer(Game* game, int rows, int cols, sfRenderWindow* window);
void game_start_multiplayer(Game* game, int rows, int cols, sfRenderWindow* window, sfTcpSocket* socket);
void game_handle_event(Game* game, const sfEvent* event);
void game_handle_event_multiplayer(Game* game, const sfEvent* event);
void game_handle_revealed_cards(Game* game, Pexeso* revealedCards[2], bool* inputDisabled, bool* waitingToHide, sfClock* revealTimer);
void game_check_pair(Pexeso* revealedCards[2], bool* waitingToHide, sfClock* revealTimer);
void game_reset_revealed_cards(Pexeso* revealedCards[2], bool* inputDisabled, bool* waitingToHide, sfClock* revealTimer);
void game_draw(Game* game, sfRenderWindow* window);
#endif //PEXESO_GAME_H
