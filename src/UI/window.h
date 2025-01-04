//
// Created by seil0 on 2. 1. 2025.
//

#ifndef PEXESO_WINDOW_H
#define PEXESO_WINDOW_H
#include <SFML/Graphics.h>
#include <SFML/Window.h>
#include <SFML/System.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "screens.h"
#include "Components/button.h"
#include "Components/header.h"
#include "Components/label.h"
#include "Components/Setters.h"
#include "../Rules/Rules.h"
#include "../GameLogic/Game.h"


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
    Label* rowLabel;
    Label* colLabel;
    Label* errorLabel;
    Label* playersLabel;
    Screen* currentScreen;
    Header* header;
    sfFont* font;
    Rules* rules;
    Game* game;
    int rowSize;
    int colSize;
    bool canStart;
} Window;

Window* window_create();
void handleClickEvents(Window* window);
void draw(Window* window, Screen currentScreen);
void windowStart(Window* window);
void windowDestroy(Window* window);

#endif //PEXESO_WINDOW_H
