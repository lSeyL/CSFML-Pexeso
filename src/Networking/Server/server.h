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

#define MAX_CLIENTS 10

typedef struct {
    sfTcpSocket* clients[MAX_CLIENTS];
    int clientCount;
    pthread_mutex_t clientMutex;
    int gridRows;
    int gridCols;
} Server;

typedef struct {
    Server* server;
    sfTcpSocket* client;
} ClientHandlerArgs;

void* handle_client(void* client_socket);

#endif //PEXESO_SERVER_H
