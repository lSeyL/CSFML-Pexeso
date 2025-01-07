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
    Pexeso* revealedPexesoCards[2];
    PexesoGrid* currentGrid;
    sfBool isGameRunning;
} Server;

typedef struct {
    Server* server;
    sfTcpSocket* client;
} ClientHandlerArgs;
void broadcastPairedCards(Server* server, int cardID1, int cardID2);
void broadcastResetCards(Server* server, int cardID1, int cardID2);
void broadcast_grid(Server* server);
void broadcast_message(Server* server, const char* message);
bool checkPairedCards(Pexeso* pexFirst, Pexeso* pexSecond);
Pexeso* findByID(Server* server, int id);
void* handle_client(void* client_socket);
#endif //PEXESO_SERVER_H
