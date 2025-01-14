//
// Created by seil0 on 2. 1. 2025.
//
#include "window.h"
#include <stdlib.h>

Window *window_create() {
    Window *window = (Window *) malloc(sizeof(Window));
    if (!window) {
        return NULL;
    }
    window->currentScreen = (Screen *) malloc(sizeof(Screen));
    if (!window->currentScreen) {
        windowDestroy(window);
        return NULL;
    }
    *window->currentScreen = MAIN_MENU;
    window->font = sfFont_createFromFile("../../Resources/Roboto-Light.ttf");
    if (!window->font) {
        printf("Window - Failed to load font\n");
        windowDestroy(window);
        return NULL;
    }
    window->renderWindow = NULL;
    window->renderWindow = sfRenderWindow_create(
            (sfVideoMode){1200, 800, 32}, "Pexeso", sfResize | sfClose, NULL);
    if (!window->renderWindow) {
        windowDestroy(window);
        return NULL;
    }
    window->backgroundTexture = sfTexture_createFromFile("../../Resources/backgroundImage.png", NULL);
    if (!window->backgroundTexture) {
        printf("Failed to load the background texture.\n");
        windowDestroy(window);
        return NULL;
    }
    window->backgroundSprite = sfSprite_create();
    if(!window->backgroundSprite) {
        printf("Failed to create the background sprite.\n");
        windowDestroy(window);
        return NULL;
    }
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
        windowDestroy(window);
        return NULL;
    }
    highlightButton(window->rowButtons, 2);
    sfVector2f columnStartPosition = {350, 200};
    window->columnButtons = setterCreateWithNumbers(&columnStartPosition, &buttonSize, 9, window->font);
    if (!window->columnButtons) {
        printf("Failed to create column buttons\n");
        windowDestroy(window);
        return NULL;
    }
    highlightButton(    window->columnButtons, 2);

    sfVector2f modeStartPosition = {350, 490};
    sfVector2f modeButtonSize = {175, 75};
    const char* modeLabels[] = {"Free", "Timed"};
    window->modeButtons = setterCreateWithName(&modeStartPosition, &modeButtonSize, modeLabels, 2, window->font);
    if (!window->modeButtons) {
        printf("Failed to create mode buttons\n");
        windowDestroy(window);
        return NULL;
    }
    highlightButton(window->modeButtons, 0);
    sfVector2f difficultyStartPosition = {350, 390};
    sfVector2f difficultyButtonSize = {175, 75};
    const char* difficultyLabels[] = {"Easy", "Medium", "Hard"};
    window->difficultyButtons = setterCreateWithName(&difficultyStartPosition, &difficultyButtonSize, difficultyLabels, 3, window->font);
    if (!window->difficultyButtons) {
        printf("Failed to create difficulty buttons\n");
        windowDestroy(window);
        return NULL;
    }
    highlightButton( window->difficultyButtons, 0);
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
    if (!window->singlePlayerButton || !window->multiplayerButton || !window->exitButton || !window->backButton
            || !window->startButton || !window->hostGameButton || !window->joinGameButton || !window->okButton) {
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

    window->rowLabel = labelCreate("Columns:", window->font, (sfVector2f) {75, 80}, 60, sfWhite);
    window->colLabel = labelCreate("Rows:", window->font, (sfVector2f) {75, 180}, 60, sfWhite);
    window->diffLabel = labelCreate("Difficulty:", window->font, (sfVector2f) {75, 380}, 60, sfWhite);
    window->modeLabel = labelCreate("Mode:", window->font, (sfVector2f) {75, 480}, 60, sfWhite);
    window->errorLabel  = labelCreate("", window->font, (sfVector2f) {280, 275}, 50, sfRed);
    window->infoLabel  = labelCreate("", window->font, (sfVector2f) {300, 275}, 75, sfWhite);

    window->spPlayerTurn  = labelCreate("", window->font, (sfVector2f) {850, 50}, 50, sfWhite);
    window->spPoints  = labelCreate("0 points", window->font, (sfVector2f) {850, 125}, 45, sfWhite);
    window->timeLabel  = labelCreate("", window->font, (sfVector2f) {850, 250}, 50, sfWhite);
    window->timeNumLabel  = labelCreate("", window->font, (sfVector2f) {850, 325}, 45, sfWhite);
    window->mpPlayersPointsLabel  = labelCreate("", window->font, (sfVector2f) {25, 25}, 30, sfWhite);
    window->playersLabel = labelCreate("Players", window->font, (sfVector2f) {950, 50}, 50, sfWhite);

    if (!window->rowLabel || !window->colLabel || !window->playersLabel || !window->errorLabel
        || !window->infoLabel  || !window->spPlayerTurn || !window->spPoints || !window->timeLabel || !window->timeNumLabel
        || !window->mpPlayersPointsLabel || !window->diffLabel || !window->modeLabel) {
        printf("Failed to create a label.\n");
        windowDestroy(window);
        return NULL;
    }
    window->rules = rulesCreate();
    window->game = gameCreate(window->renderWindow, window->rules);
    window->mp_gameFinished = false;
    window->mp_isMyTurn = false;
    window->mp_ClientIDTurn = false;
    window->mp_ClientID = -1;
    window->colSize = 4;
    window->rowSize = 4;
    window->socket = NULL;
    pthread_mutex_init(&window->socketMutex, NULL);
    window->isHost = false;
    window->closeGame = false;
    setDefaultSelectedIndex(window->difficultyButtons, 0);
    setDefaultSelectedIndex(window->modeButtons, 0);
    setDefaultSelectedIndex(window->rowButtons, 2);
    setDefaultSelectedIndex(window->columnButtons, 2);
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
                window->closeGame = true;
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
            window->rules->difficulty = getSelectedBase(window->difficultyButtons);
            window->rules->mode = getSelectedBase(window->modeButtons);
            if (buttonClicked(window->backButton, &event)) {
                printf("back\n");
                *window->currentScreen = MAIN_MENU;
            }
            if (buttonClicked(window->startButton, &event)) {
                if (checkPair(window->rules, window->rowSize, window->colSize)) {
                    printf("Single player game started.\n");
                    printf("Difficulty : %d.\n", window->rules->difficulty);
                    printf("Mode : %d.\n", window->rules->mode);
                    printf("Start with: %dx%d\n", window->rowSize, window->colSize);
                    *window->currentScreen = SINGLE_PLAYER_STARTED;
                    gameStartSingleplayer(window->game, window->colSize, window->rowSize, window->renderWindow,
                                          window->rules->difficulty, window->rules->mode);
                } else {
                    window->canStart = false;
                    char errorMessage[256];
                    sprintf(errorMessage, "Cannot start the game with %d x %d", window->rowSize, window->colSize);
                    labelSetText(window->errorLabel, errorMessage);
                    printf("Cannot start rows: %d , cols: %d\n", window->rowSize, window->colSize);
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
                labelSetText(window->infoLabel, infoMessage);
            }
            if (buttonClicked(window->joinGameButton, &event)) {
                printf("back\n");
                *window->currentScreen = MULTI_PLAYER_JOIN;
                char infoMessage[256];
                sprintf(infoMessage, " Joining a server...", window->rowSize, window->colSize);
                labelSetText(window->infoLabel, infoMessage);
            }
        }
        if (*window->currentScreen == MULTI_PLAYER_HOST)
        {
            if (buttonClicked(window->backButton, &event)) {
                *window->currentScreen = MULTI_PLAYER_HOSTJOIN;
            }
            if (buttonClicked(window->okButton, &event)) {
                printf("Hosting...\n");
                if (system("start Server.exe") == -1) {
                    printf("Failed to start the server.\n");
                    char errorMessage[256];
                    sprintf(errorMessage, "Failed to start the server.");
                    labelSetText(window->errorLabel, errorMessage);
                    return;
                }
                window->isHost = true;
                *window->currentScreen = MULTI_PLAYER;
            }
        }
        if (*window->currentScreen == MULTI_PLAYER_JOIN)
        {
            if (buttonClicked(window->backButton, &event)) {
                *window->currentScreen = MULTI_PLAYER_HOSTJOIN;
            }
            if (buttonClicked(window->okButton, &event)) {
                printf("Joining...\n");
                //*window->currentScreen = MULTI_PLAYER_TRYJOIN;
                *window->currentScreen = MULTI_PLAYER_STARTED;
                createListener(window);
            }
        }

        if (*window->currentScreen == MULTI_PLAYER) {
            setters_handleEvent(window->rowButtons, &event);
            setters_handleEvent(window->columnButtons, &event);
            setters_handleEvent(window->modeButtons, &event);
            //setters_handleEvent(window->difficultyButtons, mousePosF);
            window->rowSize = getSelected(window->rowButtons);
            window->colSize = getSelected(window->columnButtons);
            window->rules->mode = getSelectedBase(window->modeButtons);
            if (buttonClicked(window->backButton, &event)) {
                *window->currentScreen = MAIN_MENU;
            }

            if (buttonClicked(window->startButton, &event)) {
                if (checkPair(window->rules, window->rowSize, window->colSize)) {
                    printf("Starting..\n");
                    window->canStart = true;
                    *window->currentScreen = MULTI_PLAYER_STARTED;
                    if (window->isHost) {
                        printf("Hosting server...\n");
                        createListener(window);
                        sendGridToServer(window->socket, window->rowSize, window->colSize, window->rules->mode);

                    }

                } else {
                    window->canStart = false;
                    char errorMessage[256];
                    sprintf(errorMessage, "Cannot start the game with %d x %d", window->rowSize, window->colSize);
                    labelSetText(window->errorLabel, errorMessage);
                    printf("rows: %d , cols: %d\n", window->rowSize, window->colSize);
                }

            }

        }


        if (*window->currentScreen == SINGLE_PLAYER_STARTED) {
            //printf("game handle event\n");
            gameHandleEvent(window->game, &event);
            char whosTurnText[256];
            char numPointsText[256];
            if(window->game->isPlayerTurn){
                sprintf(whosTurnText, "Player's turn");
                sprintf(numPointsText, "%d points", window->game->playerPoints);
            } else {
                sprintf(whosTurnText, "Bot's turn");
                sprintf(numPointsText, "%d points", window->game->botPoints);
            }
            labelSetText(window->spPlayerTurn, whosTurnText);
            labelSetText(window->spPoints, numPointsText);
            if(window->game->mode == 2)
            {
                char timeLeftAsString[10];
                updateTimerLabel(window->game, timeLeftAsString, sizeof(timeLeftAsString));
                labelSetText(window->timeLabel, "Time left");
                labelSetText(window->timeNumLabel, timeLeftAsString);
            }
            if(window->game->isRunning)
            {
                if(window->game->win) {
                    *window->currentScreen = WIN_SCREEN;
                    char infoMessage[256];
                    bool playerWon = window->game->playerPoints > window->game->botPoints ;
                    sprintf(infoMessage, "%s won the game.",
                            playerWon ? "Player" : "Bot");
                    labelSetText(window->infoLabel, infoMessage);
                }
            }

        }
        if (*window->currentScreen == MULTI_PLAYER_STARTED) {
            //printf("game handle event\n");
            if(!window->mp_gameFinished){
                gameHandleEventMultiplayer(window->game, &event, window->mp_isMyTurn);
            } else {
                printf("Game finished\n");
                *window->currentScreen = WIN_SCREEN;

            }

        }
        if (*window->currentScreen == WIN_SCREEN) {
            if(buttonClicked(window->okButton, &event)) {
                window->closeGame = true;
            }
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
        labelDraw(window->rowLabel, window->renderWindow);
        labelDraw(window->colLabel, window->renderWindow);
        labelDraw(window->diffLabel, window->renderWindow);
        labelDraw(window->modeLabel, window->renderWindow);
        if(!window->canStart)
        {
            labelDraw(window->errorLabel, window->renderWindow);
        }


    } else if (currentScreen == MULTI_PLAYER) {
        sfRenderWindow_drawSprite(window->renderWindow, window->backgroundSprite, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->backButton->shape, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->startButton->shape, NULL);
        drawSetters(window->rowButtons, window->renderWindow);
        drawSetters(window->columnButtons, window->renderWindow);
        drawSetters(window->modeButtons, window->renderWindow);
        //drawSetters(window->difficultyButtons, window->renderWindow);
        labelDraw(window->rowLabel, window->renderWindow);
        labelDraw(window->colLabel, window->renderWindow);
        labelDraw(window->playersLabel, window->renderWindow);
        labelDraw(window->modeLabel, window->renderWindow);
        if(!window->canStart)
        {
            labelDraw(window->errorLabel, window->renderWindow);
        }
    } else if (currentScreen == MULTI_PLAYER_HOSTJOIN) {
        sfRenderWindow_drawSprite(window->renderWindow, window->backgroundSprite, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->hostGameButton->shape, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->joinGameButton->shape, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->backButton->shape, NULL);
    } else if (currentScreen == MULTI_PLAYER_HOST) {
        sfRenderWindow_drawSprite(window->renderWindow, window->backgroundSprite, NULL);
        labelDraw(window->infoLabel, window->renderWindow);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->okButton->shape, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->backButton->shape, NULL);
    } else if (currentScreen == MULTI_PLAYER_JOIN) {
        sfRenderWindow_drawSprite(window->renderWindow, window->backgroundSprite, NULL);
        labelDraw(window->infoLabel, window->renderWindow);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->okButton->shape, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->backButton->shape, NULL);
    } else if (currentScreen == SINGLE_PLAYER_STARTED) {
        sfRenderWindow_drawSprite(window->renderWindow, window->backgroundSprite, NULL);
        labelDraw(window->spPlayerTurn, window->renderWindow);
        labelDraw(window->spPoints, window->renderWindow);
        if(window->game->mode == 2) {
            labelDraw(window->timeLabel, window->renderWindow);
            labelDraw(window->timeNumLabel, window->renderWindow);
        }
        gameDraw(window->game, window->renderWindow);
    }else if (currentScreen == MULTI_PLAYER_STARTED) {
        sfRenderWindow_drawSprite(window->renderWindow, window->backgroundSprite, NULL);
        labelDraw(window->spPlayerTurn, window->renderWindow);
        labelDraw(window->spPoints, window->renderWindow);
        if(window->rules->mode == 2) {
            labelDraw(window->timeLabel, window->renderWindow);
            labelDraw(window->timeNumLabel, window->renderWindow);
        }
        gameDraw(window->game, window->renderWindow);
    }else if (currentScreen == WIN_SCREEN) {
        sfRenderWindow_drawSprite(window->renderWindow, window->backgroundSprite, NULL);
        sfRenderWindow_drawRectangleShape(window->renderWindow, window->okButton->shape, NULL);
        labelDraw(window->infoLabel, window->renderWindow);
        labelDraw(window->mpPlayersPointsLabel, window->renderWindow);


    }


}

void windowStart(Window* window) {
    if (!window || !window->renderWindow) return;
    while (sfRenderWindow_isOpen(window->renderWindow)) {
        handleClick(window);
        draw(window, *window->currentScreen);
        sfRenderWindow_display(window->renderWindow);
        if(window->closeGame) {
            sfSleep(sfMilliseconds(1000));
            break;
        }
    }
    printf("Destroying game/window.\n");
    windowDestroy(window);
}

void windowDestroy(Window* window) {
    if (!window) return;

    if (window->currentScreen) {
        free(window->currentScreen);
        window->currentScreen = NULL;
    }
    if (window->backgroundSprite) {
        sfSprite_destroy(window->backgroundSprite);
        window->backgroundSprite = NULL;
    }
    if (window->backgroundTexture) {
        sfTexture_destroy(window->backgroundTexture);
        window->backgroundTexture = NULL;
    }
    //Buttony
    if (window->singlePlayerButton) {
        buttonDestroy(window->singlePlayerButton);
        window->singlePlayerButton = NULL;
    }
    if (window->multiplayerButton) {
        buttonDestroy(window->multiplayerButton);
        window->multiplayerButton = NULL;
    }
    if (window->exitButton) {
        buttonDestroy(window->exitButton);
        window->exitButton = NULL;
    }
    if (window->backButton) {
        buttonDestroy(window->backButton);
        window->backButton = NULL;
    }
    if (window->startButton) {
        buttonDestroy(window->startButton);
        window->startButton = NULL;
    }
    if (window->hostGameButton) {
        buttonDestroy(window->hostGameButton);
        window->hostGameButton = NULL;
    }
    if (window->joinGameButton) {
        buttonDestroy(window->joinGameButton);
        window->joinGameButton = NULL;
    }
    if (window->okButton) {
        buttonDestroy(window->okButton);
        window->okButton = NULL;
    }
    if (window->header) {
        headerDestroy(window->header);
        window->header = NULL;
    }

    if (window->renderWindow) {
        sfRenderWindow_destroy(window->renderWindow);
        window->renderWindow = NULL;
    }
    //Settery
    if (window->rowButtons) {
        settersDestroy(window->rowButtons);
        window->rowButtons = NULL;
    }
    if (window->columnButtons) {
        settersDestroy(window->columnButtons);
        window->columnButtons = NULL;
    }
    if (window->modeButtons) {
        settersDestroy(window->modeButtons);
        window->modeButtons = NULL;
    }
    if (window->difficultyButtons) {
        settersDestroy(window->difficultyButtons);
        window->difficultyButtons = NULL;
    }

    if (window->rowLabel) {
        labelDestroy(window->rowLabel);
        window->rowLabel = NULL;
    }
    if (window->colLabel) {
        labelDestroy(window->colLabel);
        window->colLabel = NULL;
    }
    if (window->errorLabel) {
        labelDestroy(window->errorLabel);
        window->errorLabel = NULL;
    }
    if (window->infoLabel) {
        labelDestroy(window->infoLabel);
        window->infoLabel = NULL;
    }
    if (window->spPlayerTurn) {
        labelDestroy(window->spPlayerTurn);
        window->spPlayerTurn = NULL;
    }
    if (window->spPoints) {
        labelDestroy(window->spPoints);
        window->spPoints = NULL;
    }
    if (window->timeLabel) {
        labelDestroy(window->timeLabel);
        window->timeLabel = NULL;
    }
    if (window->timeNumLabel) {
        labelDestroy(window->timeNumLabel);
        window->timeNumLabel = NULL;
    }
    if (window->playersLabel) {
        labelDestroy(window->playersLabel);
        window->playersLabel = NULL;
    }
    if (window->mpPlayersPointsLabel) {
        labelDestroy(window->mpPlayersPointsLabel);
        window->mpPlayersPointsLabel = NULL;
    }
    if (window->diffLabel) {
        labelDestroy(window->diffLabel);
        window->diffLabel = NULL;
    }
    if (window->modeLabel) {
        labelDestroy(window->modeLabel);
        window->modeLabel = NULL;
    }

    if (window->game) {
        gameDestroy(window->game);
        window->game = NULL;
    }
    if (window->rules) {
        rulesDestroy(window->rules);
        window->rules = NULL;
    }

    if (window->socket) {
        pthread_mutex_lock(&window->socketMutex);
        sfTcpSocket_disconnect(window->socket);
        sfTcpSocket_destroy(window->socket);
        pthread_mutex_unlock(&window->socketMutex);
        window->socket = NULL;
    }
    pthread_mutex_destroy(&window->socketMutex);
    if (window->font) {
        sfFont_destroy(window->font);
        window->font = NULL;
    }
    free(window);
}

void createListener(Window* window)
{
    pthread_mutex_lock(&window->socketMutex);
    window->socket = sfTcpSocket_create();
    if (!window->socket) {
        printf("Failed to create socket\n");
        char errorMessage[256];
        sprintf(errorMessage, "Failed to create socket.");
        labelSetText(window->errorLabel, errorMessage);
        pthread_mutex_unlock(&window->socketMutex);
        return;
    }

    sfIpAddress serverAddress;
    if(window->isHost){
        char addr[10] = "127.0.0.1";
        serverAddress = sfIpAddress_fromString(addr);
    } else {
        char ip[16] = "127.0.0.1";
        printf("DEFAULT LOCAL: 127.0.0.1\n");
        //printf("Enter an IP address: ");
        //fgets(ip, sizeof(ip), stdin);
        serverAddress = sfIpAddress_fromString(ip);
    }


    sfSocketStatus status = sfTcpSocket_connect(window->socket, serverAddress, 53000, sfSeconds(5.0f));
    if (status != sfSocketDone) {
        printf("Failed to connect to the server\n");
        *window->currentScreen = MAIN_MENU;
        /*
        sfTcpSocket_destroy(window->socket);
        window->socket = NULL;
        return;
         */
    }
    pthread_mutex_unlock(&window->socketMutex);
    pthread_t listenerThread;
    if (pthread_create(&listenerThread, NULL, serverListenerThread, window) != 0) {
        printf("Failed to create listener thread\n");
    } else {
        printf("Listener thread started\n");
    }
    pthread_detach(listenerThread);
}

void* serverListenerThread(void* arg)
{
    Window* window = (Window*)arg;
    char readBuffer[256];
    static char accumulatedBuffer[1024];
    size_t accumLen = 0;
    sfTcpSocket_setBlocking(window->socket, sfFalse);
    while (1)
    {
        pthread_mutex_lock(&window->socketMutex);
        if (!window->socket) {
            printf("Socket is null, exiting thread.\n");
            pthread_mutex_unlock(&window->socketMutex);
            break;
        }
        size_t received = 0;
        sfSocketStatus status = sfTcpSocket_receive(window->socket,readBuffer,sizeof(readBuffer) - 1,&received);
        pthread_mutex_unlock(&window->socketMutex);
        if (status == sfSocketDisconnected) {
            printf("Disconnected from server\n");
            break;
        }
        else if (status == sfSocketNotReady) {
            sfSleep(sfMilliseconds(100));
            continue;
        }
        else if (status == sfSocketError) {
            printf("Socket error occurred\n");
            break;
        }
        else if (status == sfSocketDone)
        {
            printf("Received data. \n");
            readBuffer[received] = '\0';
            if (accumLen + received < sizeof(accumulatedBuffer)) {
                memcpy(accumulatedBuffer + accumLen, readBuffer, received + 1);
                accumLen += received;
            }
            else {
                accumLen = 0;
                continue;
            }
            char* start = accumulatedBuffer;
            while (1) {
                char* newlinePos = strchr(start, '\n');
                if (!newlinePos) {
                    break;
                }
                *newlinePos = '\0';
                printf("Processing with - %s \n", start);
                processLine(window, start);
                start = newlinePos + 1;
            }
            size_t leftover = strlen(start);
            memmove(accumulatedBuffer, start, leftover + 1);
            accumLen = leftover;
        }
        else {
            printf("Status error\n");
        }
    }

    return NULL;
}

 void processLine(Window* window, const char* line)
{
    printf("[Client] - Message from server: %s\n", line);

    if (strncmp(line, "GRID", 4) == 0) {
        handleGridCommand(window, line);
    }
    else if(strncmp(line, "TIME", 4) == 0){
        handleTime(window, line);
    }
    else if(strncmp(line, "PING", 4) == 0){
        handlePing(window, line);
    }
    else if(strncmp(line, "PLAYER", 6) == 0){
        handlePoints(window, line);
    }
    else if (strncmp(line, "START_GAME", 10) == 0) {
        handleStartGameCommand(window, line);
    }
    else if (strncmp(line, "CARD_CLICK", 10) == 0) {
        handleCardClickCommand(window, line);
    }
    else if (strncmp(line, "DATA", 4) == 0) {
        handleGridData(window, line);
    }
    else if (strncmp(line, "COMPLETE", 8) == 0) {
        handleComplete(window);
    }
    else if (strncmp(line, "PAIRED_CARDS", 12) == 0) {
        handlePairedCards(window, line);
    }
    else if (strncmp(line, "HIDE_CARDS", 10) == 0) {
        handleResetCards(window, line);
    }
    else if (strncmp(line, "CLIENT", 6) == 0) {
        handleClientID(window, line);
    }
    else if (strncmp(line, "TURN", 4) == 0) {
        handleClientTurn(window, line);
    }
    else if (strncmp(line, "WIN", 3) == 0) {
        handleGameFinish(window, line);
    }
    else if (strncmp(line, "UPDATE", 6) == 0) {
        handleUpdatePoints(window, line);
    }
    else {
        printf("Unknown request: %s\n", line);
    }
}

 void handleGridCommand(Window* window, const char* line)
{
    int rows, cols, timed;
    if (sscanf(line, "GRID %d %d %d", &rows, &cols, &timed) == 3) {
        if (!window->game->isRunning) {
            pthread_mutex_lock(&window->socketMutex);
            window->colSize = cols;
            window->rowSize = rows;
            window->rules->mode = timed;
            calculateGridLayoutMultiplayer(window);
            pthread_mutex_unlock(&window->socketMutex);
        }
    }
}

 void handleStartGameCommand(Window* window, const char* line)
{
    printf("Received START_GAME\n");
    /*
    if (!window->game->isRunning) {
        pthread_mutex_lock(&window->socketMutex);
        gameStartMultiplayer(window->game, window->rowSize, window->colSize, window->renderWindow, window->socket);
        pthread_mutex_unlock(&window->socketMutex);
    }
     */
}

 void handleCardClickCommand(Window* window, const char* line)
{
    int cardID;
    if (sscanf(line, "CARD_CLICK %d", &cardID) == 1) {
        printf("Server says card clicked: ID=%d\n", cardID);

        for (int i = 0; i < window->game->grid->rows * window->game->grid->columns; ++i) {
            Pexeso* card = window->game->grid->pexesoObjects[i];
            if (card->id == cardID) {
                if (!card->revealed && !card->wasFound) {
                    reveal(card);
                }
                printf("Revealing card ID=%d locally\n", cardID);
                break;
            }
        }
    }
}
void handlePoints(Window* window, const char* line) {
    printf("[Debug] Processing WIN message:\n%s\n", line);
    const char* currentLabelText = labelGetText(window->mpPlayersPointsLabel);
    char finalMessage[512];

    snprintf(finalMessage, sizeof(finalMessage), "%s", currentLabelText);
    int playerId, points;
    int charsRead = 0;
    if (sscanf(line, " PLAYER %d: %d\n%n", &playerId, &points, &charsRead) == 2) {
        char playerInfo[64];
        snprintf(playerInfo, sizeof(playerInfo), "Player %d: %d\n", playerId, points);
        strncat(finalMessage, playerInfo, sizeof(finalMessage) - strlen(finalMessage) - 1);
    } else {
        printf("[Error] Failed to parse line: %s\n", line);
    }
    labelSetText(window->mpPlayersPointsLabel, finalMessage);
    printf("[Debug] Final points displayed:\n%s\n", finalMessage);
}

 void handleGridData(Window* window, const char* line)
{
    int cardID, color;
    char label;
    if (sscanf(line, "DATA %d %c %u", &cardID, &label, &color) == 3) {
        pthread_mutex_lock(&window->socketMutex);
        int row = cardID / window->colSize;
        int col = cardID % window->colSize;

        sfVector2f position = {window->mpGridStart.x + col * (window->mpTileSize.x + 10),
                               window->mpGridStart.y + row * (window->mpTileSize.y + 10)};

        Pexeso* pex = pexesoCreate(&position, &window->mpTileSize, sfBlack, getColorFromInteger(color), label);
        setID(pex, cardID);
        printf("Card received: %d %c %u\n", getID(pex), getLabel(pex), getIntegerBasedOnColor(*getColor(pex)));
        if (!window->game->grid) {
            printf("Grid not yet initialized.\nCreating a new Grid.. \n");
            calculateGridLayout(window->game,window->renderWindow);
            window->game->grid = pexesoGridCreate(window->rowSize, window->colSize, window->game->gridStartPosition,
                                                  window->game->tileSize, false);
        }
        window->game->grid->pexesoObjects[cardID] = pex;

        pthread_mutex_unlock(&window->socketMutex);
    }
}

 void handleComplete(Window* window)
{
    printf("All grid data sent from the server.\n");
    //
    const char* message = "OK\n";
    sfSocketStatus status = sfTcpSocket_send(window->socket, message, strlen(message));
    if (status != sfSocketDone) {
        printf("Failed to send msg to server.\n");
    }
    window->game->gridLoaded = true;
    if (!window->game->isRunning) {
        pthread_mutex_lock(&window->socketMutex);
        gameStartMultiplayer(window->game, window->rowSize, window->colSize, window->renderWindow, window->socket);
        pthread_mutex_unlock(&window->socketMutex);
    }
}

 void handlePairedCards(Window* window, const char* line)
{
    int cardID_1, cardID_2;
    if (sscanf(line, "PAIRED_CARDS %d %d", &cardID_1, &cardID_2) == 2) {
        printf("Received matching IDs ID_1=%d, ID_2=%d\n", cardID_1, cardID_2);
        Pexeso* pex1 = window->game->grid->pexesoObjects[cardID_1];
        Pexeso* pex2 = window->game->grid->pexesoObjects[cardID_2];
        hide(pex1);
        hide(pex2);
        setWasFound(pex1);
        setWasFound(pex2);
    }
}

void handleResetCards(Window* window, const char* line)
{
    int cardID_1, cardID_2;
    if (sscanf(line, "HIDE_CARDS %d %d", &cardID_1, &cardID_2) == 2) {
        printf("Hiding cards with IDs ID_1=%d, ID_2=%d\n", cardID_1, cardID_2);
        Pexeso* pex1 = window->game->grid->pexesoObjects[cardID_1];
        Pexeso* pex2 = window->game->grid->pexesoObjects[cardID_2];

        hide(pex1);
        hide(pex2);
    }
}
void handleClientID(Window* window, const char* line) {
    int id;
    if (sscanf(line, "CLIENT %d", &id) == 1) {
        window->mp_ClientID = id;
        printf("Received clientID = %d = %d\n", id, window->mp_ClientID);
    }
}

void handleClientTurn(Window* window, const char* line) {
    int id;
    if (sscanf(line, "TURN %d", &id) == 1) {
        char turnText[64];
        printf("My turn: %d, Server turn: %d \n",window->mp_ClientID, id );
        if(window->mp_ClientID == id) {
            printf("My turn.\n");
            window->mp_isMyTurn = true;
            sprintf(turnText, "Your turn");
            labelSetText(window->spPlayerTurn, turnText);
        } else {
            printf("Not my turn.\n");
            window->mp_isMyTurn = false;
            sprintf(turnText, "Player %d turn ", id+1);
            labelSetText(window->spPlayerTurn, turnText);
        }
    }
}
void handleGameFinish(Window* window, const char* line)
{
    printf("Received finish");
    if (sscanf(line, "WIN") == 0)
    {
        window->mp_gameFinished = true;
        char pointsMessage[256] = "The game has finished.\n";
        sfVector2f pos = {pos.x = 250, pos.y = 275};
        labelSetPosition(window->infoLabel, pos);
        labelSetText(window->infoLabel, pointsMessage);
    }
}
void handleUpdatePoints(Window* window, const char* line) {
    int points;
    if (sscanf(line, "UPDATE %d", &points) == 1) {
        char pointsText[64];
        sprintf(pointsText, "%d points", points);
        labelSetText(window->spPoints, pointsText);
    }
}
void handlePing(Window* window, const char* line) {
    if (sscanf(line, "PING") == 0) {
        const char* message = "PONG\n";
        sfSocketStatus status = sfTcpSocket_send(window->socket, message, strlen(message));
        if (status != sfSocketDone) {
            printf("Failed to send msg to server.\n");
        }
    }
}

void handleTime(Window* window, const char* line) {
    int timeLeft = 0;
    if (sscanf(line, "TIME %d", &timeLeft) == 1) {
        char timeText[64];
        sprintf(timeText, "%02d:%02d", timeLeft / 60,timeLeft % 60);
        labelSetText(window->timeNumLabel, timeText);
    }

}


void sendGridToServer(sfTcpSocket* socket, int rows, int cols, int timed) {
    if (!socket) {
        printf("Socket is NULL. Cannot send grid.\n");
        return;
    }

    char message[256];
    snprintf(message, sizeof(message), "GRID %d %d %d\n", rows, cols, timed);

    sfSocketStatus status = sfTcpSocket_send(socket, message, strlen(message));
    if (status != sfSocketDone) {
        printf("Failed to send grid to server\n");
    } else {
        printf("Grid sent to server: %s\n", message);
    }
}



void calculateGridLayoutMultiplayer(Window *window)
{
    float windowWidth  = sfRenderWindow_getSize(window->renderWindow).x;
    float windowHeight = sfRenderWindow_getSize(window->renderWindow).y;
    float rightPadding  = 400.f;
    float leftPadding   = 10.f;
    float topPadding    = 50.f;
    float bottomPadding = 100.f;
    float usableWidth  = windowWidth  - leftPadding - rightPadding;
    float usableHeight = windowHeight - topPadding  - bottomPadding;
    float tileWidth  = usableWidth  / window->colSize;
    float tileHeight = usableHeight / window->rowSize;
    float tileSize   = (tileWidth < tileHeight) ? tileWidth : tileHeight;
    if (tileSize * window->rowSize > usableHeight
        || tileSize * window->colSize > usableWidth)
    {
        float tileSizeByRows = usableHeight / window->rowSize;
        float tileSizeByCols = usableWidth  / window->colSize;
        tileSize = (tileSizeByRows < tileSizeByCols) ? tileSizeByRows : tileSizeByCols;
    }
    if (tileSize < 50.f) {
        tileSize = 50.f;
    }
    float gridWidth  = tileSize * window->colSize;
    float gridHeight = tileSize * window->rowSize;
    float startX = leftPadding + (usableWidth  - gridWidth)  / 2.f;
    float startY = topPadding  + (usableHeight - gridHeight) / 2.f;

    sfVector2f startPosition = { startX, startY };
    sfVector2f size          = { tileSize, tileSize };

    window->mpGridStart = startPosition;
    window->mpTileSize = size;
    printf("[calculateGridLayoutMultiplayer] Window: %.0fx%.0f, tile=%.2f\n",
           windowWidth, windowHeight, tileSize);
    printf("StartPos=(%.0f, %.0f), Grid=%dx%d\n",
           startPosition.x, startPosition.y, window->colSize, window->rowSize);
}
