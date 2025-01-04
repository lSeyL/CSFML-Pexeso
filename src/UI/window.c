//
// Created by seil0 on 2. 1. 2025.
//
#include "window.h"
#include <stdlib.h>

Window* window_create() {
    Window* window = (Window*)malloc(sizeof(Window));
    window->currentScreen = (Screen*)malloc(sizeof(Screen));
    window->font = sfFont_createFromFile("../../Resources/Roboto-Light.ttf");
    if (!window->font) {
        printf("Failed to load font\n");
        return NULL;
    }
    *window->currentScreen = MAIN_MENU;
    window->renderWindow = sfRenderWindow_create(
            (sfVideoMode){1200, 800, 32}, "Pexeso", sfResize | sfClose, NULL);
    if (!window->renderWindow) {
        free(window);
        return NULL;
    }
    window->backgroundTexture = sfTexture_createFromFile("../../Resources/backgroundImage.png", NULL);
    if (!window->backgroundTexture) {
        printf("Failed to load background texture\n");
        sfRenderWindow_destroy(window->renderWindow);
        free(window);
        return NULL;
    }
    window->backgroundSprite = sfSprite_create();
    sfSprite_setTexture(window->backgroundSprite, window->backgroundTexture, sfTrue);
    window->header = headerCreate("../../Resources/header_logo.png", 0, 0);
    if (!window->header) {
        printf("Failed to create header\n");
        windowDestroy(window);
        return NULL;
    }
    sfVector2f rowStartPosition = {350, 100};
    sfVector2f buttonSize = {50, 50};
    window->rowButtons = setterCreateWithNumbers(&rowStartPosition, &buttonSize, 9, window->font);
    if (! window->rowButtons) {
        printf("Failed to create row buttons\n");
        return NULL;
    }

    sfVector2f columnStartPosition = {350, 200};
    window->columnButtons = setterCreateWithNumbers(&columnStartPosition, &buttonSize, 9, window->font);
    if (!window->columnButtons) {
        printf("Failed to create column buttons\n");
        return NULL;
    }
    sfVector2f modeStartPosition = {350, 490};
    sfVector2f modeButtonSize = {175, 75};
    const char* modeLabels[] = {"Free", "Timed"};
    window->modeButtons = setterCreateWithName(&modeStartPosition, &modeButtonSize, modeLabels, 2, window->font);
    if (!window->modeButtons) {
        printf("Failed to create mode buttons\n");
        return NULL;
    }
    sfVector2f difficultyStartPosition = {350, 390};
    sfVector2f difficultyButtonSize = {175, 75};
    const char* difficultyLabels[] = {"Easy", "Medium", "Hard"};
    window->difficultyButtons = setterCreateWithName(&difficultyStartPosition, &difficultyButtonSize, difficultyLabels, 3, window->font);
    if (!window->difficultyButtons) {
        printf("Failed to create difficulty buttons\n");
        return NULL;
    }
    sfVector2u windowSize = sfRenderWindow_getSize(window->renderWindow);
    sfVector2f headerSize = (sfVector2f) {
            sfTexture_getSize(window->header->texture).x,
            sfTexture_getSize(window->header->texture).y
    };
    headerSetPos(window->header,
                        (windowSize.x - headerSize.x) / 2.0f,
                        (windowSize.y - headerSize.y) / 2.0f - 200);





    window->singlePlayerButton = buttonCreate("../../Resources/button_singleplayer.png", 0, 0);
    window->multiplayerButton = buttonCreate("../../Resources/button_multiplayer.png", 0, 0);
    window->exitButton = buttonCreate("../../Resources/button_exit.png", 0, 0);
    window->backButton = buttonCreate("../../Resources/button_back.png", 100, 700);
    window->startButton = buttonCreate("../../Resources/button_start.png", 939, 700);
    window->hostGameButton = buttonCreate("../../Resources/button_host-a-game.png", 271, 500);
    window->joinGameButton = buttonCreate("../../Resources/button_join-a-game.png", 674, 500);
    window->okButton = buttonCreate("../../Resources/button_ok.png", 532, 400);
    if (!window->singlePlayerButton || !window->multiplayerButton || !window->exitButton) {
        printf("Failed to create a button.\n");
        windowDestroy(window);
        return NULL;
    }
    sfVector2f singlePlayerSize = sfRectangleShape_getSize(window->singlePlayerButton->shape);
    sfVector2f multiplayerSize = sfRectangleShape_getSize(window->multiplayerButton->shape);
    sfVector2f exitSize = sfRectangleShape_getSize(window->exitButton->shape);

    float verticalSpacing = 50.0f;
    float totalHeight = singlePlayerSize.y + multiplayerSize.y + exitSize.y + (2 * verticalSpacing) - 100;
    float startY = (windowSize.y - totalHeight) / 2.0f;

    sfVector2f singlePlayerPosition = {
            (windowSize.x - singlePlayerSize.x) / 2.0f,
            startY
    };

    sfVector2f multiplayerPosition = {
            (windowSize.x - multiplayerSize.x) / 2.0f,
            startY + singlePlayerSize.y + verticalSpacing
    };

    sfVector2f exitPosition = {
            (windowSize.x - exitSize.x) / 2.0f,
            startY + singlePlayerSize.y + multiplayerSize.y + (2 * verticalSpacing)
    };
    sfRectangleShape_setPosition(window->singlePlayerButton->shape, singlePlayerPosition);
    sfRectangleShape_setPosition(window->multiplayerButton->shape, multiplayerPosition);
    sfRectangleShape_setPosition(window->exitButton->shape, exitPosition);

    window->rowLabel = label_create("Columns:", window->font, (sfVector2f){75, 80}, 60, sfWhite);
    window->colLabel = label_create("Rows:", window->font, (sfVector2f){75, 180}, 60, sfWhite);
    window->playersLabel = label_create("Players", window->font, (sfVector2f){950, 50}, 50, sfWhite);
    window->errorLabel  = label_create("", window->font, (sfVector2f){280, 275}, 50, sfRed);
    window->infoLabel  = label_create("", window->font, (sfVector2f){280, 275}, 75, sfWhite);
    window->rules = rules_create();
    window->game = game_create(window->renderWindow, window->rules);
    window->colSize = 2;
    window->rowSize = 2;
    window->socket = NULL;
    return window;
}

void handleClick(Window *window) {
    sfEvent event;
    while (sfRenderWindow_pollEvent(window->renderWindow, &event)) {
        if (event.type == sfEvtClosed) {
            sfRenderWindow_close(window->renderWindow);
        }

        //menu
        if (*window->currentScreen == MAIN_MENU) {
            if (buttonClicked(window->singlePlayerButton, &event)) {
                *window->currentScreen = SINGLE_PLAYER;
            }

            if (buttonClicked(window->multiplayerButton, &event)) {
                *window->currentScreen = MULTI_PLAYER_HOSTJOIN;
            }

            if (buttonClicked(window->exitButton, &event)) {
                *window->currentScreen = EXIT_SCREEN;
                sfRenderWindow_close(window->renderWindow);
            }
        }
        //single
        if (*window->currentScreen == SINGLE_PLAYER) {
            setters_handleEvent(window->rowButtons, &event);
            setters_handleEvent(window->columnButtons, &event);
            setters_handleEvent(window->modeButtons, &event);
            setters_handleEvent(window->difficultyButtons, &event);
            window->rowSize = getSelected(window->rowButtons);
            window->colSize = getSelected(window->columnButtons);
            if (buttonClicked(window->backButton, &event)) {
                printf("back\n");
                *window->currentScreen = MAIN_MENU;
            }
            if (buttonClicked(window->startButton, &event)) {
                if (checkPair(window->rules, window->rowSize, window->colSize)) {
                    printf("start\n");
                    *window->currentScreen = SINGLE_PLAYER_STARTED;
                    game_start(window->game, window->colSize, window->rowSize, window->renderWindow);
                } else {
                    window->canStart = false;
                    char errorMessage[256];
                    sprintf(errorMessage, "Cannot start the game with %d x %d", window->rowSize, window->colSize);
                    label_set_text(window->errorLabel, errorMessage);
                    printf("cannot start rows: %d , cols: %d\n", window->rowSize, window->colSize);
                }

            }

        }
        //multi
        if (*window->currentScreen == MULTI_PLAYER_HOSTJOIN)
        {
            if (buttonClicked(window->backButton, &event)) {
                printf("back\n");
                *window->currentScreen = MAIN_MENU;
            }
            if (buttonClicked(window->hostGameButton, &event)) {
                printf("back\n");
                *window->currentScreen = MULTI_PLAYER_HOST;
                char infoMessage[256];
                sprintf(infoMessage, "Hosting a server...", window->rowSize, window->colSize);
                label_set_text(window->infoLabel, infoMessage);
            }
            if (buttonClicked(window->joinGameButton, &event)) {
                printf("back\n");
                *window->currentScreen = MULTI_PLAYER_JOIN;
                char infoMessage[256];
                sprintf(infoMessage, "Joining a server...", window->rowSize, window->colSize);
                label_set_text(window->infoLabel, infoMessage);
            }
        }
        if (*window->currentScreen == MULTI_PLAYER_HOST)
        {
            if (buttonClicked(window->backButton, &event)) {
                *window->currentScreen = MULTI_PLAYER_HOSTJOIN;
            }
            if (buttonClicked(window->okButton, &event)) {
                printf("HOSTING\n");
                *window->currentScreen = MULTI_PLAYER;
                if (system("start Server.exe") == -1) {
                    printf("Failed to start the server.\n");
                }
            }
        }
        if (*window->currentScreen == MULTI_PLAYER_JOIN)
        {
            if (buttonClicked(window->backButton, &event)) {
                *window->currentScreen = MULTI_PLAYER_HOSTJOIN;
            }
            if (buttonClicked(window->okButton, &event)) {
                printf("Hosting..\n");
                *window->currentScreen = MULTI_PLAYER_TRYJOIN;
                window->socket = sfTcpSocket_create();
                if (!window->socket) {
                    printf("Failed to create socket\n");
                    return;
                }
                sfIpAddress serverAddress = sfIpAddress_fromString("127.0.0.1");
                sfSocketStatus status = sfTcpSocket_connect(window->socket, serverAddress, 53000, sfSeconds(5.0f));
                if (status != sfSocketDone) {
                    printf("Failed to connect to server\n");
                    sfTcpSocket_destroy(window->socket);
                    window->socket = NULL;
                }
            }
        }

        if (*window->currentScreen == MULTI_PLAYER) {
            setters_handleEvent(window->rowButtons, &event);
            setters_handleEvent(window->columnButtons, &event);
            setters_handleEvent(window->modeButtons, &event);
            //setters_handleEvent(window->difficultyButtons, mousePosF);
            window->rowSize = getSelected(window->rowButtons);
            window->colSize = getSelected(window->columnButtons);
            if (buttonClicked(window->backButton, &event)) {
                *window->currentScreen = MAIN_MENU;
            }

            if (buttonClicked(window->startButton, &event)) {
                if (checkPair(window->rules, window->rowSize, window->colSize)) {
                    printf("Starting..\n");
                    window->canStart = true;
                    *window->currentScreen = MULTI_PLAYER_STARTED;
                    game_start(window->game, window->colSize, window->rowSize, window->renderWindow);
                } else {
                    window->canStart = false;
                    char errorMessage[256];
                    sprintf(errorMessage, "Cannot start the game with %d x %d", window->rowSize, window->colSize);
                    label_set_text(window->errorLabel, errorMessage);
                    printf("cannot start rows: %d , cols: %d\n", window->rowSize, window->colSize);
                }

            }

        }


        if (*window->currentScreen == SINGLE_PLAYER_STARTED) {
            //printf("game handle event\n");
            game_handle_event(window->game, &event);
        }
        if (*window->currentScreen == MULTI_PLAYER_STARTED) {
            //printf("game handle event\n");
            game_handle_event(window->game, &event);
        }
    }
}


void draw(Window* window, Screen currentScreen) {
    sfRenderWindow_clear(window->renderWindow, sfBlack);
    if (currentScreen == MAIN_MENU) {
        sfRenderWindow_drawSprite(window->renderWindow, window->backgroundSprite, NULL);
        sfRenderWindow_drawSprite(window->renderWindow, window->header->sprite, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->singlePlayerButton->shape, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->multiplayerButton->shape, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->exitButton->shape, NULL);
    } else if (currentScreen == SINGLE_PLAYER) {
        sfRenderWindow_drawSprite(window->renderWindow, window->backgroundSprite, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->backButton->shape, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->startButton->shape, NULL);
        drawSetters(window->rowButtons, window->renderWindow);
        drawSetters(window->columnButtons, window->renderWindow);
        drawSetters(window->modeButtons, window->renderWindow);
        drawSetters(window->difficultyButtons, window->renderWindow);
        label_draw(window->rowLabel, window->renderWindow);
        label_draw(window->colLabel, window->renderWindow);
        if(!window->canStart)
        {
            label_draw(window->errorLabel, window->renderWindow);
        }

    } else if (currentScreen == MULTI_PLAYER) {
        sfRenderWindow_drawSprite(window->renderWindow, window->backgroundSprite, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->backButton->shape, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->startButton->shape, NULL);
        drawSetters(window->rowButtons, window->renderWindow);
        drawSetters(window->columnButtons, window->renderWindow);
        drawSetters(window->modeButtons, window->renderWindow);
        //drawSetters(window->difficultyButtons, window->renderWindow);
        label_draw(window->rowLabel, window->renderWindow);
        label_draw(window->colLabel, window->renderWindow);
        label_draw(window->playersLabel, window->renderWindow);
        if(!window->canStart)
        {
            label_draw(window->errorLabel, window->renderWindow);
        }
    } else if (currentScreen == MULTI_PLAYER_HOSTJOIN) {
        sfRenderWindow_drawSprite(window->renderWindow, window->backgroundSprite, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->hostGameButton->shape, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->joinGameButton->shape, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->backButton->shape, NULL);
    } else if (currentScreen == MULTI_PLAYER_HOST) {
        sfRenderWindow_drawSprite(window->renderWindow, window->backgroundSprite, NULL);
        label_draw(window->infoLabel, window->renderWindow);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->okButton->shape, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->backButton->shape, NULL);
    } else if (currentScreen == MULTI_PLAYER_JOIN) {
        sfRenderWindow_drawSprite(window->renderWindow, window->backgroundSprite, NULL);
        label_draw(window->infoLabel, window->renderWindow);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->okButton->shape, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->backButton->shape, NULL);
    } else if (currentScreen == SINGLE_PLAYER_STARTED) {
        sfRenderWindow_drawSprite(window->renderWindow, window->backgroundSprite, NULL);
        game_draw(window->game, window->renderWindow);
    }else if (currentScreen == MULTI_PLAYER_STARTED) {
        sfRenderWindow_drawSprite(window->renderWindow, window->backgroundSprite, NULL);
        game_draw(window->game, window->renderWindow);
    }

}

void windowStart(Window* window) {
    if (!window || !window->renderWindow) return;
    while (sfRenderWindow_isOpen(window->renderWindow)) {
        handleClick(window);
        draw(window, *window->currentScreen);
        sfRenderWindow_display(window->renderWindow);
    }
}

void windowDestroy(Window* window) {
    if (!window) return;
    if (window->currentScreen) free(window->currentScreen);
    if (window->backgroundSprite) sfSprite_destroy(window->backgroundSprite);
    if (window->backgroundTexture) sfTexture_destroy(window->backgroundTexture);
    //Buttons
    if (window->singlePlayerButton) buttonDestroy(window->singlePlayerButton);
    if (window->multiplayerButton)buttonDestroy(window->multiplayerButton);
    if (window->exitButton)buttonDestroy(window->exitButton);
    if (window->hostGameButton)buttonDestroy(window->hostGameButton);
    if (window->joinGameButton)buttonDestroy(window->joinGameButton);
    //
    if (window->header)headerDestroy(window->header);
    if (window->renderWindow) sfRenderWindow_destroy(window->renderWindow);
    if (window->rowButtons)settersDestroy(window->rowButtons);
    if (window->columnButtons)settersDestroy(window->columnButtons);
    if (window->modeButtons)settersDestroy(window->modeButtons);
    if (window->difficultyButtons)settersDestroy(window->difficultyButtons);

    if (window->rowLabel)label_destroy(window->rowLabel);
    if (window->colLabel)label_destroy(window->colLabel);
    if (window->errorLabel)label_destroy(window->errorLabel);
    if (window->infoLabel)label_destroy(window->infoLabel);
    if (window->font)sfFont_destroy(window->font);

    if (window->game)game_destroy(window->game);
    if (window->rules)rules_destroy(window->rules);

    if (window->socket) {
        sfTcpSocket_destroy(window->socket);
    }
    free(window);
}