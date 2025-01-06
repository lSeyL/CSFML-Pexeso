//
// Created by seil0 on 2. 1. 2025.
//

#ifndef PEXESO_WINDOW_H
#define PEXESO_WINDOW_H
#include <SFML/Graphics.h>
#include <SFML/Window.h>
#include <SFML/System.h>
#include <SFML/Network.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

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
    Button* hostGameButton;
    Button* joinGameButton;
    Button* okButton;
    SetterButtons* rowButtons;
    SetterButtons* columnButtons;
    SetterButtons* modeButtons;
    SetterButtons* difficultyButtons;
    sfTexture* backgroundTexture;
    sfSprite* backgroundSprite;
    Label* rowLabel;
    Label* colLabel;
    Label* errorLabel;
    Label* infoLabel;
    Label* spPlayerTurn;
    Label* spPoints;
    Label* timeLabel;
    Label* timeNumLabel;
    Label* playersLabel;
    Screen* currentScreen;
    Header* header;
    sfFont* font;
    Rules* rules;
    Game* game;
    sfTcpSocket* socket;
    pthread_mutex_t socketMutex;
    int rowSize;
    int colSize;
    bool canStart;
    bool isHost;
    bool isNetworkingThreadRunning;
} Window;

Window* window_create();
void handleClickEvents(Window* window);
void draw(Window* window, Screen currentScreen);
void windowStart(Window* window);
void windowDestroy(Window* window);

void create_listener(Window* window);
void* server_listener_thread(void* arg);
//void* networking_thread(void* arg);
void send_grid_to_server(sfTcpSocket* socket, int rows, int cols);

#endif //PEXESO_WINDOW_H
