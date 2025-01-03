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
    window->rules = rules_create();
    return window;
}
void handleClick(Window* window, sfVector2f mousePosF, Screen* currentScreen) {
    if(*window->currentScreen == MAIN_MENU) {
    if (buttonClicked(window->singlePlayerButton, mousePosF)) {
        printf("Single Player Game Started\n");
        *window->currentScreen = SINGLE_PLAYER;
    }

    if (buttonClicked(window->multiplayerButton, mousePosF)) {
        printf("Multiplayer Game Started\n");
        *window->currentScreen = MULTI_PLAYER;
    }

    if (buttonClicked(window->exitButton, mousePosF)) {
        printf("Exiting Game\n");
        *window->currentScreen = EXIT_SCREEN;
        sfRenderWindow_close(window->renderWindow);
    }
    }
    if(*window->currentScreen == SINGLE_PLAYER) {
        setters_handleEvent(window->rowButtons, mousePosF);
        setters_handleEvent(window->columnButtons, mousePosF);
        setters_handleEvent(window->modeButtons, mousePosF);
        setters_handleEvent(window->difficultyButtons, mousePosF);
        window->rowSize = getSelected(window->rowButtons);
        window->colSize = getSelected(window->columnButtons);
        if (buttonClicked(window->backButton, mousePosF)) {
            printf("back\n");
            *window->currentScreen = MAIN_MENU;
        }

        if (buttonClicked(window->startButton, mousePosF)) {
            if(checkPair(window->rules, window->rowSize, window->colSize)) {
                printf("start\n");
                *window->currentScreen = SINGLE_PLAYER_STARTED;
            } else {
                printf("cannot start rows: %d , cols: %d\n", window->rowSize, window->colSize);
            }

        }
    }


}

void handleClickEvents(Window* window, Screen* currentScreen) {
    sfEvent event;
    while (sfRenderWindow_pollEvent(window->renderWindow, &event)) {
        if (event.type == sfEvtClosed) {
            sfRenderWindow_close(window->renderWindow);
        }
        if (event.type == sfEvtMouseButtonPressed) {
            sfVector2i mousePos = sfMouse_getPositionRenderWindow(window->renderWindow);
            sfVector2f mousePosF = (sfVector2f){mousePos.x, mousePos.y};
            handleClick(window, mousePosF, currentScreen);
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
    } else if (currentScreen == MULTI_PLAYER) {
        sfRenderWindow_drawSprite(window->renderWindow, window->backgroundSprite, NULL);
    }
    sfRenderWindow_display(window->renderWindow);
}

void windowStart(Window* window) {
    if (!window || !window->renderWindow) return;
    while (sfRenderWindow_isOpen(window->renderWindow)) {
        handleClickEvents(window, window->currentScreen);
        draw(window, *window->currentScreen);
    }
}

void windowDestroy(Window* window) {
    if (!window) return;
    if (window->currentScreen) free(window->currentScreen);
    if (window->backgroundSprite) sfSprite_destroy(window->backgroundSprite);
    if (window->backgroundTexture) sfTexture_destroy(window->backgroundTexture);
    if (window->singlePlayerButton) buttonDestroy(window->singlePlayerButton);
    if (window->multiplayerButton)buttonDestroy(window->multiplayerButton);
    if (window->exitButton)buttonDestroy(window->exitButton);
    if (window->header)headerDestroy(window->header);
    if (window->font)sfFont_destroy(window->font);
    if (window->renderWindow) sfRenderWindow_destroy(window->renderWindow);
    settersDestroy(window->rowButtons);
    settersDestroy(window->columnButtons);
    settersDestroy(window->modeButtons);
    settersDestroy(window->difficultyButtons);
    rules_destroy(window->rules);

    free(window);
}