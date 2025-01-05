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
#include "../../GameLogic/Pexeso.h"
#define MAX_CLIENTS 10

typedef struct {
    sfTcpSocket* clients[MAX_CLIENTS];
    pthread_mutex_t clientMutex;
    int clientCount;
    int gridRows;
    int gridCols;
    PexesoGrid* currentGrid;
    sfBool isGameRunning;
} Server;

typedef struct {
    Server* server;
    sfTcpSocket* client;
} ClientHandlerArgs;

void* handle_client(void* client_socket);
void send_full_grid_state(Server* server, sfTcpSocket* client);

#endif //PEXESO_SERVER_H
