//
// Created by seil0 on 5. 1. 2025.
//

#ifndef PEXESO_SERVER_H
#define PEXESO_SERVER_H

#include <SFML/Network.h>

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#include "../../GameLogic/PexesoGrid.h"

#define MAX_CLIENTS 10

typedef struct {
    sfTcpSocket* clients[MAX_CLIENTS];
    pthread_mutex_t clientMutex;
    int clientCount;
    int currentClientTurn;
    int gridRows;
    int gridCols;
    int pexesoToCompare;
    int points[MAX_CLIENTS];
    Pexeso* revealedPexesoCards[2];
    PexesoGrid* currentGrid;
    bool isGameRunning;
    bool isGameFinished;
    bool isServerRunning;
    bool isTimed;
    sfClock* pingClock;
    sfClock* gameClock;
    int timeLimit;
} Server;

typedef struct {
    Server* server;
    sfTcpSocket* client;
    char name[32];
    int id;
} ClientArg;

void broadcastPairedCards(Server* server, int cardID1, int cardID2);
void broadcastResetCards(Server* server, int cardID1, int cardID2);
void broadcast_grid(Server* server);
void broadcast_clientID(Server *server, ClientArg* args);
void broadcast_winMessage(Server *server);
void broadcast_message(Server* server, const char* message);
void checkGameState(Server* server);
void addPointsToCurrentClient(Server* server, bool addPoints);
bool checkPairedCards(Pexeso* pexFirst, Pexeso* pexSecond);
Pexeso* findByID(Server* server, int id);
void nextTurn(Server* server, bool calculateNext);
void* handleClient(void* client_socket);
#endif //PEXESO_SERVER_H
