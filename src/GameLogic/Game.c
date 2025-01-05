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
        game_reset_revealed_cards(revealedCards, &inputDisabled, &waitingToHide, revealTimer);
        return;
    }

    pexeso_grid_handle_click(game->grid, event);
    game_handle_revealed_cards(game, revealedCards, &inputDisabled, &waitingToHide, revealTimer);
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
        game_check_pair(revealedCards, waitingToHide, revealTimer);
    }
}

void game_check_pair(Pexeso* revealedCards[2], bool* waitingToHide, sfClock* revealTimer) {
    if (getIntegerBasedOnColor(*getColor(revealedCards[0])) == getIntegerBasedOnColor(*getColor(revealedCards[1])) &&
        getLabel(revealedCards[0]) == getLabel(revealedCards[1])) {
        printf("Match found!\n");
        hide(revealedCards[0]);
        hide(revealedCards[1]);
        setColor(revealedCards[0], sfTransparent);
        setColor(revealedCards[1], sfTransparent);
        setWasFound(revealedCards[0]);
        setWasFound(revealedCards[1]);
        revealedCards[0] = revealedCards[1] = NULL;
    } else {
        printf("Not a pair.\n");
    }
    sfClock_restart(revealTimer);
    *waitingToHide = true;
}

void game_reset_revealed_cards(Pexeso* revealedCards[2], bool* inputDisabled, bool* waitingToHide, sfClock* revealTimer) {
    if (*waitingToHide && sfTime_asSeconds(sfClock_getElapsedTime(revealTimer)) > 1.0f) {
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
    }
}

void game_start_multiplayer(Game* game, int rows, int cols, sfRenderWindow* window, sfTcpSocket* socket) {
    if (game->isRunning) {
        printf("Game is already running; skipping reinitialization.\n");
        return;
    }

    game->isMultiplayer = true;
    game->socket = socket;
    printf("Initializing multiplayer grid: rows=%d, cols=%d\n", rows, cols);
    game_start_singleplayer(game, rows, cols, window);

    // Send game configuration to server
    char message[256];
    sprintf(message, "START_GAME %d %d", rows, cols);
    sfSocketStatus status = sfTcpSocket_send(socket, message, strlen(message));
    if (status != sfSocketDone) {
        game->isRunning = false;
        printf("Failed to send game configuration to server\n");
        return;
    }

    game->isRunning = true;
    printf("Multiplayer game started\n");
}

void game_handle_event_multiplayer(Game* game, const sfEvent* event) {
    if (!game || !game->isRunning || !game->isMultiplayer) return;

    char buffer[256];
    size_t received;

    // Receive updates from the server
    sfSocketStatus status = sfTcpSocket_receive(game->socket, buffer, sizeof(buffer) - 1, &received);
    if (status == sfSocketDone) {
        buffer[received] = '\0';
        printf("Server: %s\n", buffer);

        // Process server message (e.g., update board)
    } else if (status == sfSocketDisconnected) {
        printf("Disconnected from server\n");
        game->isRunning = false;
    }

    // Handle local events
    game_handle_event(game, event);

    // Send card click to server
    if (event->type == sfEvtMouseButtonPressed) {
        sfVector2f mousePos = {event->mouseButton.x, event->mouseButton.y};
        char message[64];
        sprintf(message, "CARD_CLICK %.0f %.0f", mousePos.x, mousePos.y);
        sfTcpSocket_send(game->socket, message, strlen(message));
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

