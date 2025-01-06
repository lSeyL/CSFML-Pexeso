//
// Created by seil0 on 3. 1. 2025.
//
#include "Game.h"


Game* game_create() {
    Game* game = (Game*)malloc(sizeof(Game));
    if (!game) return NULL;

    game->grid = NULL;
    game->rowSize = 0;
    game->colSize = 0;
    game->isRunning = false;
    game->isMultiplayer = false;
    game->socket = NULL;
    return game;
}

void game_destroy(Game* game) {
    if (game) {
        if (game->grid) {
            pexeso_grid_destroy(game->grid);
        }
        free(game);
    }
}

void game_start_singleplayer(Game* game, int rows, int cols, sfRenderWindow* window) {
    game->rowSize = rows;
    game->colSize = cols;
    game->isPlayerTurn = true;

    // Define screen dimensions and padding
    float windowWidth = sfRenderWindow_getSize(window).x;
    float windowHeight = sfRenderWindow_getSize(window).y;
    float rightPadding = 400; // Space on the right side of the screen
    float leftPadding = 10;   // Space on the left side of the screen
    float topPadding = 50;    // Space at the top of the screen
    float bottomPadding = 100; // Space at the bottom of the screen

    // Calculate the usable area for the grid
    float usableWidth = windowWidth - leftPadding - rightPadding;
    float usableHeight = windowHeight - topPadding - bottomPadding;

    // Calculate tile size based on the grid dimensions
    float tileWidth = usableWidth / cols;
    float tileHeight = usableHeight / rows;
    float tileSize = tileWidth < tileHeight ? tileWidth : tileHeight;

    // Ensure tiles are smaller if the grid doesn't fit
    if (tileSize * rows > usableHeight || tileSize * cols > usableWidth) {
        tileSize = (usableHeight / rows) < (usableWidth / cols) ? (usableHeight / rows) : (usableWidth / cols);
    }

    // Clamp the tile size (optional, remove if unnecessary)
    if (tileSize < 50.0f) tileSize = 50.0f;

    // Calculate grid dimensions
    float gridWidth = tileSize * cols;
    float gridHeight = tileSize * rows;

    // Start position (centered within the usable area)
    float startX = leftPadding + (usableWidth - gridWidth) / 2;
    float startY = topPadding + (usableHeight - gridHeight) / 2;

    sfVector2f startPosition = {startX, startY};
    sfVector2f size = {tileSize, tileSize};
    // Create the grid
    game->grid = pexeso_grid_create(rows, cols, startPosition, size);

    if (!game->grid) {
        printf("Failed to create Pexeso grid\n");
        return;
    }
    game->isRunning = true;
    pexeso_grid_shuffle(game->grid);
}

void game_handle_event(Game* game, const sfEvent* event) {
    if (!game || !game->isRunning) return;

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

    if (inputDisabled) {
        game_reset_revealed_cards(game,revealedCards, &inputDisabled, &waitingToHide, revealTimer);
        if (!waitingToHide && !inputDisabled) {
            if(!game->isMultiplayer) {
            game->isPlayerTurn = !game->isPlayerTurn; // Switch turn
            printf("Turn switched: %s\n", game->isPlayerTurn ? "Player" : "Bot");
            }
        }
        return;
    }

    if (game->isPlayerTurn) {
        if (event->type == sfEvtMouseButtonPressed) {
            pexeso_grid_handle_click(game->grid, event);
            game_handle_revealed_cards(game, revealedCards, &inputDisabled, &waitingToHide, revealTimer);
        }
    }

    // Bot's turn
   // printf("PLAYER TURN: %d %d %d\n",game->isPlayerTurn, waitingToHide ,inputDisabled);
    if (!game->isMultiplayer && !game->isPlayerTurn ) {
        printf("1\n");
        game->isPlayerTurn = true;
        bot_take_turn(game, revealTimer);
    }
}

void bot_take_turn(Game* game, sfClock* revealTimer) {
    if (!game || !game->isRunning) return;

    printf("Bot's turn!\n");

    Pexeso* revealedCards[2] = {NULL, NULL};
    int revealedCount = 0;

    // Randomly reveal two cards
    for (int i = 0; i < game->grid->rows * game->grid->columns; ++i) {
        Pexeso* card = game->grid->pexesoObjects[i];
        if (!card->revealed && revealedCount < 2) {
            reveal(card);
            revealedCards[revealedCount++] = card;
            printf("Bot revealed card: ID=%d, Label=%c\n", card->id, card->label);
        }

        if (revealedCount == 2) break;
    }

    // Check if the revealed cards match
    if (revealedCount == 2) {
        bool waitingToHide = false;
        bool inputDisabled = true;

        // Pass the game instance to the functions
        game_check_pair(game, revealedCards, &waitingToHide, revealTimer);
        sfSleep(sfSeconds(1)); // Simulate delay
        game_reset_revealed_cards(game, revealedCards, &inputDisabled, &waitingToHide, revealTimer);

        // Switch turn back to the player
        game->isPlayerTurn = true;
        printf("Turn switched: Player's turn\n");
    }
}

void game_handle_revealed_cards(Game* game, Pexeso* revealedCards[2], bool* inputDisabled, bool* waitingToHide, sfClock* revealTimer) {
    int revealedCount = 0;

    for (int i = 0; i < game->grid->rows * game->grid->columns; ++i) {
        Pexeso* card = game->grid->pexesoObjects[i];
        if (getRevealed(card) && revealedCount < 2) {
            revealedCards[revealedCount++] = card;
        }
    }

    if (revealedCount == 2) {
        *inputDisabled = true;
        game->disableSend = true;  // Disable sending further clicks
        game_check_pair(game,revealedCards, waitingToHide, revealTimer);
    } else {
        game->disableSend = false; // Allow sending for new clicks
    }
}

void game_check_pair(Game* game,Pexeso* revealedCards[2], bool* waitingToHide, sfClock* revealTimer) {
    if (getIntegerBasedOnColor(*getColor(revealedCards[0])) == getIntegerBasedOnColor(*getColor(revealedCards[1])) &&
        getLabel(revealedCards[0]) == getLabel(revealedCards[1])) {
        printf("Match found!\n");
        hide(revealedCards[0]);
        hide(revealedCards[1]);
        setColor(revealedCards[0], sfTransparent);
        setColor(revealedCards[1], sfTransparent);
        setWasFound(revealedCards[0]);
        setWasFound(revealedCards[1]);
        /*
        if(game->isMultiplayer) {
            char message[256];
            snprintf(message, sizeof(message), "PAIRED_CARDS %d %d", revealedCards[0]->id, revealedCards[1]->id);
            sfTcpSocket_send(socket, message, strlen(message));
        }
         */
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

void game_start_multiplayer(Game* game, int rows, int cols, sfRenderWindow* window, sfTcpSocket* socket) {
    if (game->isRunning) {
        return;
    }

    game->isMultiplayer = true;
    game->socket = socket;
    printf("Initializing multiplayer grid: rows=%d, cols=%d\n", rows, cols);
    game_start_singleplayer(game, rows, cols, window);
 char message[256];
 snprintf(message, sizeof(message), "GRID %d %d", rows, cols);
 sfTcpSocket_send(socket, message, strlen(message));
    game->isRunning = true;
    printf("Multiplayer game started\n");
}

void game_handle_event_multiplayer(Game* game, const sfEvent* event) {
    if (!game || !game->isRunning || !game->isMultiplayer) return;


    // Handle local events
    game_handle_event(game, event);
    // Send card click to server
    if (event->type == sfEvtMouseButtonPressed) {
        sfVector2f mousePos = {event->mouseButton.x, event->mouseButton.y};

        for (int i = 0; i < game->grid->rows * game->grid->columns; ++i) {
            Pexeso* card = game->grid->pexesoObjects[i];
            sfFloatRect cardBounds = sfRectangleShape_getGlobalBounds(card->shape);

            if (sfFloatRect_contains(&cardBounds, mousePos.x, mousePos.y)) {


                // Send only the card ID

                char message[64];
                snprintf(message, sizeof(message), "CARD_CLICK %d", card->id);
                sfSocketStatus status = sfTcpSocket_send(game->socket, message, strlen(message));
                if (status != sfSocketDone) {
                    printf("Failed to send card click to server\n");
                } else {
                    //printf("Card clicked SENT: ID=%d\n", card->id);
                }

                break; // Stop after processing the first valid click
            }


        }
    }
}

void game_draw(Game* game, sfRenderWindow* window) {
    if (!game) {
        printf("Game not initialized.\n");
        return;
    }
    //printf("game->isRunning: %d\n", game->isRunning);
    if (!game->isRunning) {
        //printf("Game not running; cannot draw.\n");
        return;
    }
    //printf("Drawing grid with rows=%d, cols=%d\n", game->rowSize, game->colSize);
    pexeso_grid_draw(game->grid, window);
}

