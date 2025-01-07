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
#include <stdlib.h>
#include <string.h>

#include "PexesoGrid.h"
#include "../Rules/Rules.h"
#include "../UI/Components/label.h"

#define MAX_CARDS 10
typedef struct {
    int cardID;
    char label;
    unsigned int color;
    bool wasFound;
} MemoryCard;

typedef struct {
    sfVector2f tileSize;
    sfVector2f gridStartPosition;
    PexesoGrid* grid;
    int rowSize;
    int colSize;
    sfTcpSocket* socket;
    bool gridLoaded;
    int gridDataCount;
    bool isHost;
    bool isMultiplayer;
    bool isRunning;
    bool disableSend;
    bool win;
    int playerPoints;
    int mode;
    //bot
    bool isPlayerTurn;
    int difficulty;
    int botPoints;
    MemoryCard memory[MAX_CARDS];
    int memoryCount;

    sfClock* gameClock;
    int timeLimit;
} Game;



Game* game_create(sfRenderWindow* renderWindow, Rules* rules);
void debug_grid_mapping(Game* game);
void calculateGridLayout(Game* game, sfRenderWindow* window);
void setupLabels();
void game_destroy(Game* game);
bool checkWinCondition(Game* game);
void game_check_pair(Game* game,Pexeso* revealedCards[2], bool* waitingToHide, sfClock* revealTimer);
void game_reset_revealed_cards(Game* game,Pexeso* revealedCards[2], bool* inputDisabled, bool* waitingToHide, sfClock* revealTimer);
void game_handle_revealed_cards(Game* game, Pexeso* revealedCards[2], bool* inputDisabled, bool* waitingToHide, sfClock* revealTimer);
void game_start_singleplayer(Game* game, int rows, int cols, sfRenderWindow* window, int difficulty, int mode);
void game_draw(Game* game, sfRenderWindow* window);
//bot
void bot_take_turn(Game* game, sfClock* revealTimer, Pexeso* revealedCards[2], bool* inputDisabled, bool* waitingToHide);
void bot_remember_card(Game* game, int cardID, char label, unsigned int color);
void bot_mark_cards_found(Game* game, int cardID1, int cardID2);
void bot_sync_memory(Game* game);
//timed
int get_remaining_time(Game* game);
void update_timer_label(Game* game, char* buffer, size_t bufferSize);
//mp
void game_handle_event(Game* game, const sfEvent* event);
void game_start_multiplayer(Game* game, int rows, int cols, sfRenderWindow* window, sfTcpSocket* socket);
void game_handle_event_multiplayer(Game* game, const sfEvent* event);
void sendGridToServer(Game* game, sfTcpSocket* socket);
#endif //PEXESO_GAME_H
