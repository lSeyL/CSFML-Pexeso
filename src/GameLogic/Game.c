//
// Created by seil0 on 3. 1. 2025.
//
#include "Game.h"


Game* gameCreate(sfRenderWindow* renderWindow, Rules* rules) {
    Game* game = (Game*)malloc(sizeof(Game));
    if (!game) return NULL;
    game->grid = NULL;
    game->socket = NULL;
    game->gameClock = NULL;
    game->rowSize = 0;
    game->colSize = 0;
    game->playerPoints = 0;
    game->botPoints = 0;
    game->isRunning = false;
    game->isMultiplayer = false;
    game->gridLoaded = false;
    game->win = false;
    return game;
}

void gameDestroy(Game* game) {
    printf("Destroying grid.\n");
    if (game) {
        if (game->grid) {
            pexesoGridDestroy(game->grid);
            game->grid = NULL;
        }
        if(game->gameClock){
            sfClock_destroy(game->gameClock);
            game->gameClock = NULL;
        }
        free(game);
        game = NULL;
    }
}

bool checkWinCondition(Game* game) {
    if (!game || !game->grid) {
        return false;
    }
    if(game->mode == 2 ) {
        if(getRemainingTime(game) == 0) {
            printf("Out of time.\n");
            game->win = true;
            return true;
        }
    }
    for (int i = 0; i < game->grid->rows * game->grid->columns; ++i) {
        Pexeso* card = game->grid->pexesoObjects[i];
        if (!card->wasFound) {
            return false;
        }
    }
    game->win = true;
    return true;
}

void gameStartSingleplayer(Game* game, int rows, int cols, sfRenderWindow* window, int difficulty, int mode) {
    game->rowSize = rows;
    game->colSize = cols;
    game->isPlayerTurn = true;
    game->difficulty = difficulty;
    game->mode = mode;
    game->memoryCount = 0;
    memset(game->memory, -1, sizeof(game->memory));
    if(game->mode == 2 ){
        game->timeLimit = 10 * rows + 10 * cols;
        printf("Time limit set to: %d \n", game->timeLimit);
        game->gameClock = sfClock_create();
        if (!game->gameClock) {
            printf("Failed to create game clock.\n");
            return;
        }
    }
    calculateGridLayout(game, window);
    game->grid = pexesoGridCreate(rows, cols, game->gridStartPosition, game->tileSize, true);
    if (!game->grid) {
        printf("Failed to create Pexeso grid\n");
        return;
    }

    //debug_grid_mapping(game);
    game->isRunning = true;
}

void gameHandleEvent(Game* game, const sfEvent* event) {
    if (!game || !game->isRunning) return;

    if (checkWinCondition(game)) {
        printf("Game finished.\n");
        return;
    }

    static sfClock* revealTimer = NULL;
    static bool waitingToHide = false;
    static bool inputDisabled = false;

    if (!revealTimer) {
        revealTimer = sfClock_create();
        if (!revealTimer) {
            printf("Failed to create clock\n");
            return;
        }
    }

    static Pexeso* revealedCards[2] = {NULL, NULL};

    if (waitingToHide && sfTime_asSeconds(sfClock_getElapsedTime(revealTimer)) > 2.0f) {
        printf("Hiding cards\n");
        if (revealedCards[0] && revealedCards[1]) {
            setColor(revealedCards[0], sfBlack);
            hide(revealedCards[0]);
            setColor(revealedCards[1], sfBlack);
            hide(revealedCards[1]);
        }
        revealedCards[0] = revealedCards[1] = NULL;
        waitingToHide = false;
        inputDisabled = false;
        if (!game->isMultiplayer) {
            game->isPlayerTurn = !game->isPlayerTurn;
            printf("Turn switched: %s\n", game->isPlayerTurn ? "Player" : "Bot");
        }
    }



    if (inputDisabled) {
        return;
    }

    if (game->isPlayerTurn) {
        if (event->type == sfEvtMouseButtonPressed) {
            pexesoGridHandleClick(game->grid, event);
            gameHandleRevealedCards(game, revealedCards, &inputDisabled, &waitingToHide, revealTimer);
        }
    } else {
        printf("Bot turn\n");
        if (!game->isMultiplayer && !waitingToHide && !inputDisabled) {
            bot_makeTurn(game, revealTimer, revealedCards, &inputDisabled, &waitingToHide);
            if (!waitingToHide && !inputDisabled) {
                game->isPlayerTurn = true;
                printf("Turn switched back to Player.\n");
            }
        }
    }
}



void bot_makeTurn(Game* game, sfClock* revealTimer, Pexeso *revealedCards[2], bool* inputDisabled, bool* waitingToHide) {
    if (!game || !game->isRunning || *inputDisabled || *waitingToHide) return;

    printf("Bot's turn!\n");

    int revealedCount = 0;
    int gridSize = game->grid->rows * game->grid->columns;

    if (game->difficulty > 1) {
        printf("Difficulty: %d\n", game->difficulty);
        bot_sync(game);
        for (int i = 0; i < game->memoryCount; ++i) {
            Pexeso* card1 = game->grid->pexesoObjects[game->memory[i].cardID];
            for (int j = i + 1; j < game->memoryCount; ++j) {
                Pexeso* card2 = game->grid->pexesoObjects[game->memory[j].cardID];
                if (!card1->wasFound && !card2->wasFound &&
                    game->memory[i].label == game->memory[j].label &&
                    game->memory[i].color == game->memory[j].color) {
                    printf("Match found in memory.");
                    reveal(card1);
                    revealedCards[revealedCount++] = card1;
                    reveal(card2);
                    revealedCards[revealedCount++] = card2;
                    break;
                } else {
                    printf("No match in memory.");
                }
            }

            if (revealedCount == 2) break;
        }
    }

    if (revealedCount < 2) {
        while(revealedCount < 2){
            int random = rand() % gridSize;
            Pexeso* card = game->grid->pexesoObjects[random];
            if (!card->wasFound && !card->revealed) {
                reveal(card);
                bot_rememberCard(game, card->id, card->label, getIntegerBasedOnColor(*getColor(card)));
                revealedCards[revealedCount++] = card;
                printf("Bot revealed card: ID=%d, Label=%c\n", card->id, card->label);
            }
            if (revealedCount == 2) break;
        }
    }

    if (revealedCount == 2) {
        *inputDisabled = true;
        gameCheckPair(game, revealedCards, waitingToHide, revealTimer);
    }
}
void bot_rememberCard(Game* game, int cardID, char label, unsigned int color) {
    for (int i = 0; i < game->memoryCount; ++i) {
        if (game->memory[i].cardID == cardID) return;
    }
    if (game->memoryCount < MAX_CARDS) {
        game->memory[game->memoryCount++] = (MemoryCard){
                .cardID = cardID,
                .label = label,
                .color = color,
                .wasFound = false
        };
        printf("Bot remembered card: ID=%d, Label=%c, Color=%u\n", cardID, label, color);
    }
}

void bot_markCardsFound(Game* game, int cardID1, int cardID2) {
    for (int i = 0; i < game->memoryCount; ++i) {
        if (game->memory[i].cardID == cardID1 || game->memory[i].cardID == cardID2) {
            printf("Bot found the cards, setting to true.\n");
            game->memory[i].wasFound = true;
        }
    }
}
void bot_sync(Game* game) {
    int writeIndex = 0;

    for (int i = 0; i < game->memoryCount; ++i) {
        Pexeso* card = game->grid->pexesoObjects[game->memory[i].cardID];
        if (!card->wasFound) {
            game->memory[writeIndex++] = game->memory[i];
        } else {
            printf("Removing found card from memory: ID=%d, Label=%c\n",
                   game->memory[i].cardID, game->memory[i].label);
        }
    }
    game->memoryCount = writeIndex;
}
void debug_grid_mapping(Game* game) {
    printf("Grid Mapping:\n");
    for (int i = 0; i < game->grid->rows * game->grid->columns; ++i) {
        Pexeso* card = game->grid->pexesoObjects[i];
        printf("Index=%d, ID=%d, Label=%c, Color=%u, wasFound=%s\n",
               i, card->id, card->label, getIntegerBasedOnColor(*getColor(card)),
               card->wasFound ? "true" : "false");
    }
}

void gameHandleRevealedCards(Game* game, Pexeso *revealedCards[2], bool* inputDisabled, bool* waitingToHide, sfClock* revealTimer) {
    int revealedCount = 0;

    for (int i = 0; i < game->grid->rows * game->grid->columns; ++i) {
        Pexeso* card = game->grid->pexesoObjects[i];
        if (getRevealed(card) && revealedCount < 2) {
            revealedCards[revealedCount++] = card;
            if(game->difficulty == 3) {
                printf("Bot remembered player's turn.");
                bot_rememberCard(game, card->id, card->label, getIntegerBasedOnColor(*getColor(card)));
            }

        }
    }

    if (revealedCount == 2) {
        *inputDisabled = true;
        game->disableSend = true;
        gameCheckPair(game, revealedCards, waitingToHide, revealTimer);
    } else {
        game->disableSend = false;
    }

}

void gameCheckPair(Game* game, Pexeso *revealedCards[2], bool* waitingToHide, sfClock* revealTimer) {
    if (getIntegerBasedOnColor(*getColor(revealedCards[0])) == getIntegerBasedOnColor(*getColor(revealedCards[1])) &&
        getLabel(revealedCards[0]) == getLabel(revealedCards[1])) {
        if(game->isPlayerTurn) {
            game->playerPoints += 100;
        } else {
            game->botPoints += 100;
            bot_markCardsFound(game, revealedCards[0]->id, revealedCards[1]->id);
        }
        printf("Bot [%d points], Player [%d points]\n",game->botPoints, game->playerPoints);
        printf("Match found!\n");
        hide(revealedCards[0]);
        hide(revealedCards[1]);
        setWasFound(revealedCards[0]);
        setWasFound(revealedCards[1]);

        if (game->isMultiplayer) {
            char message[256];
            snprintf(message, sizeof(message), "PAIRED_CARDS %d %d\n", getID(revealedCards[0]), getID(revealedCards[1]));
            sfTcpSocket_send(game->socket, message, strlen(message));
        }

        revealedCards[0] = revealedCards[1] = NULL;
    } else {
        printf("Not a pair.\n");
    }
    sfClock_restart(revealTimer);
    *waitingToHide = true;
}

void game_reset_revealed_cards(Game* game,Pexeso* revealedCards[2], bool* inputDisabled, bool* waitingToHide, sfClock* revealTimer) {
    if (*waitingToHide && sfTime_asSeconds(sfClock_getElapsedTime(revealTimer)) > 2.0f) {
        printf("Hiding cards\n");
        if (revealedCards[0] && revealedCards[1]) {
            setColor(revealedCards[0], sfBlack);
            hide(revealedCards[0]);
            setColor(revealedCards[1], sfBlack);
            hide(revealedCards[1]);
        }
        revealedCards[0] = revealedCards[1] = NULL;
        *waitingToHide = false;
        *inputDisabled = false;
        game->disableSend = false;
    }
}
int getRemainingTime(Game* game) {
    if (!game->gameClock) return 0;
    sfTime elapsed = sfClock_getElapsedTime(game->gameClock);
    int remainingTime = game->timeLimit - sfTime_asSeconds(elapsed);
    return (remainingTime > 0) ? remainingTime : 0;
}
void updateTimerLabel(Game* game, char* buffer, size_t bufferSize) {
    int remainingTime = getRemainingTime(game);
    snprintf(buffer, bufferSize, "%02d:%02d", remainingTime / 60, remainingTime % 60);
}

void gameStartMultiplayer(Game* game, int rows, int cols, sfRenderWindow* window, sfTcpSocket* socket) {
    if (game->isRunning) {
        printf("Game already running\n");
        return;
    }
    game->isMultiplayer = true;
    game->socket = socket;
    game->gridLoaded = false;
    game->gridDataCount = 0;

    // log
    /*
    for (int i = 0; i < game->grid->rows * game->grid->columns; ++i) {
        printf("[Card grid]: %d %c %u\n", getID(game->grid->pexesoObjects[i]),
               getLabel(game->grid->pexesoObjects[i]),
               getIntegerBasedOnColor(*getColor(game->grid->pexesoObjects[i])));
    }
     */
    game->isRunning = true;

}

void gameHandleEventMultiplayer(Game *game, const sfEvent *event, bool isMyTurn) {
    if (!game || !game->isRunning || !game->isMultiplayer) return;

    if (isMyTurn) {
        if (event->type == sfEvtMouseButtonPressed) {
            sfVector2f mousePos = {event->mouseButton.x, event->mouseButton.y};
            for (int i = 0; i < game->grid->rows * game->grid->columns; ++i) {
                Pexeso *card = game->grid->pexesoObjects[i];
                sfFloatRect cardBounds = sfRectangleShape_getGlobalBounds(card->shape);
                if (sfFloatRect_contains(&cardBounds, mousePos.x, mousePos.y)) {
                    char message[64];
                    snprintf(message, sizeof(message), "CARD_CLICK %d\n", card->id);
                    sfSocketStatus status = sfTcpSocket_send(game->socket, message, strlen(message));
                    if (status != sfSocketDone) {
                        printf("Failed to send card click to server\n");
                    } else {
                        //printf("Card clicked SENT: ID=%d\n", card->id);
                    }
                    break;
                }


            }
        }
    }
}

void calculateGridLayout(Game* game, sfRenderWindow* window) {
    float windowWidth = sfRenderWindow_getSize(window).x;
    float windowHeight = sfRenderWindow_getSize(window).y;
    float rightPadding = 400;
    float leftPadding = 10;
    float topPadding = 50;
    float bottomPadding = 100;

    float usableWidth = windowWidth - leftPadding - rightPadding;
    float usableHeight = windowHeight - topPadding - bottomPadding;

    float tileWidth = usableWidth / game->colSize;
    float tileHeight = usableHeight / game->rowSize;
    float tileSize = tileWidth < tileHeight ? tileWidth : tileHeight;

    if (tileSize * game->rowSize > usableHeight || tileSize * game->colSize > usableWidth) {
        tileSize = (usableHeight / game->rowSize) < (usableWidth / game->colSize) ? (usableHeight / game->rowSize) : (usableWidth / game->colSize);
    }

    if (tileSize < 50.0f) tileSize = 50.0f;

    float gridWidth = tileSize * game->colSize;
    float gridHeight = tileSize * game->rowSize;

    float startX = leftPadding + (usableWidth - gridWidth) / 2;
    float startY = topPadding + (usableHeight - gridHeight) / 2;

    sfVector2f startPosition = {startX, startY};
    sfVector2f size = {tileSize, tileSize};

    game->tileSize = size;
    game->gridStartPosition = startPosition;
}

void gameDraw(Game* game, sfRenderWindow* window) {
    if (!game && !game->gridLoaded) {
        printf("Game not initialized.\n");
        return;
    }
    //printf("game->isRunning: %d\n", game->isRunning);
    if (!game->isRunning) {
        //printf("Game not running; cannot draw.\n");
        return;
    }
    //printf("Drawing grid with rows=%d, cols=%d\n", game->rowSize, game->colSize);
    pexesoGridDraw(game->grid, window);
}

