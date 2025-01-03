//
// Created by seil0 on 2. 1. 2025.
//

#ifndef PEXESO_WINDOW_H
#define PEXESO_WINDOW_H
#include <SFML/Graphics.h>
#include <SFML/Window.h>
#include <SFML/System.h>
#include <stdio.h>
#include "screens.h"
#include "Components/button.h"
#include "Components/header.h"
#include "Components/Setters.h"
#include "../Rules/Rules.h"

typedef struct {
    sfRenderWindow* renderWindow;
    Button* singlePlayerButton;
    Button* multiplayerButton;
    Button* exitButton;
    Button* backButton;
    Button* startButton;
    SetterButtons* rowButtons;
    SetterButtons* columnButtons;
    SetterButtons* modeButtons;
    SetterButtons* difficultyButtons;
    sfTexture* backgroundTexture;
    sfSprite* backgroundSprite;
    Screen* currentScreen;
    Header* header;
    sfFont* font;
    Rules* rules;
    int rowSize;
    int colSize;
} Window;

Window* window_create();
void handleClick(Window* window, sfVector2f mousePosF, Screen* currentScreen);
void handleClickEvents(Window* window, Screen* currentScreen);
void draw(Window* window, Screen currentScreen);
void windowStart(Window* window);
void windowDestroy(Window* window);

#endif //PEXESO_WINDOW_H
