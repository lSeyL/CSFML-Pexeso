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
    PexesoGrid* grid; //
    int rowSize; //
    int colSize; //
    sfTcpSocket* socket; //
    bool gridLoaded;
    int gridDataCount;
    bool isHost;
    bool isMultiplayer;
    bool isRunning;
    bool disableSend;
    bool win; //
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



Game* gameCreate(sfRenderWindow* renderWindow, Rules* rules);
//void debug_grid_mapping(Game* game);
void calculateGridLayout(Game* game, sfRenderWindow* window);
//void setupLabels();
void gameDestroy(Game* game);
bool checkWinCondition(Game* game);
void gameCheckPair(Game* game, Pexeso *revealedCards[2], bool* waitingToHide, sfClock* revealTimer);
//void game_reset_revealed_cards(Game* game,Pexeso* revealedCards[2], bool* inputDisabled, bool* waitingToHide, sfClock* revealTimer);
void gameHandleRevealedCards(Game* game, Pexeso *revealedCards[2], bool* inputDisabled, bool* waitingToHide, sfClock* revealTimer);
void gameStartSingleplayer(Game* game, int rows, int cols, sfRenderWindow* window, int difficulty, int mode);
void gameDraw(Game* game, sfRenderWindow* window);
//bot
void bot_makeTurn(Game* game, sfClock* revealTimer, Pexeso *revealedCards[2], bool* inputDisabled, bool* waitingToHide);
void bot_rememberCard(Game* game, int cardID, char label, unsigned int color);
void bot_markCardsFound(Game* game, int cardID1, int cardID2);
void bot_sync(Game* game);
//timed
int getRemainingTime(Game* game);
void updateTimerLabel(Game* game, char* buffer, size_t bufferSize);
//mp
void gameHandleEvent(Game* game, const sfEvent* event);
void gameStartMultiplayer(Game* game, int rows, int cols, sfRenderWindow* window, sfTcpSocket* socket);
void gameHandleEventMultiplayer(Game* game, const sfEvent* event, bool isMyTurn);
#endif //PEXESO_GAME_H
