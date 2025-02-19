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
    sfRenderWindow* renderWindow; //
    Button* singlePlayerButton; //
    Button* multiplayerButton; //
    Button* exitButton; //
    Button* backButton; //
    Button* startButton; //
    Button* hostGameButton; //
    Button* joinGameButton; //
    Button* okButton; //
    SetterButtons* rowButtons; //
    SetterButtons* columnButtons; //
    SetterButtons* modeButtons; //
    SetterButtons* difficultyButtons; //
    sfTexture* backgroundTexture; //
    sfSprite* backgroundSprite; //
    Label* rowLabel; //
    Label* colLabel; //
    Label* diffLabel; //
    Label* modeLabel; //
    Label* errorLabel; //
    Label* infoLabel; //
    Label* spPlayerTurn; //
    Label* spPoints; //
    Label* timeLabel; //
    Label* timeNumLabel; //
    Label* playersLabel; //
    Label* mpPlayersPointsLabel; //
    Header* header; //
    sfFont* font; //
    Screen* currentScreen; //
    sfTcpSocket* socket; //

    Rules* rules; //
    Game* game; //
    pthread_mutex_t socketMutex; //
    int rowSize;
    int colSize;
    bool canStart;
    bool isHost;
    bool isNetworkingThreadRunning;
    sfVector2f mpTileSize;
    sfVector2f mpGridStart;
    bool mp_gameFinished;
    bool mp_isMyTurn;
    int mp_ClientIDTurn;
    int mp_ClientID;
    char playerName[32];
    bool closeGame;
} Window;

Window* window_create();
void handleClick(Window* window);
void draw(Window* window, Screen currentScreen);
void windowStart(Window* window);
void windowDestroy(Window* window);

void createListener(Window* window);
void* serverListenerThread(void* arg);

//Handlery
void handleComplete(Window* window);
void calculateGridLayoutMultiplayer(Window *window);
void processLine(Window* window, const char* line);
void handleGridCommand(Window* window, const char* line);
void handleStartGameCommand(Window* window, const char* line);
void handleCardClickCommand(Window* window, const char* line);
void handleGameFinish(Window* window, const char* line);
void handleGridData(Window* window, const char* line);
void handleResetCards(Window* window, const char* line);
void handlePairedCards(Window* window, const char* line);
void handleClientID(Window* window, const char* line);
void handleClientTurn(Window* window, const char* line);
void handleUpdatePoints(Window* window, const char* line);
void handlePoints(Window* window, const char* line);
void handlePing(Window* window, const char* line);
void handleTime(Window* window, const char* line);
void sendGridToServer(sfTcpSocket* socket, int rows, int cols, int timed);
void sendNameToServer(sfTcpSocket* socket);
#endif //PEXESO_WINDOW_H
