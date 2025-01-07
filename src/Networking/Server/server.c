//
// Created by seil0 on 1. 1. 2025.
//

#include "server.h"

void broadcast_message(Server* server, const char* message) {
    pthread_mutex_lock(&server->clientMutex);
    printf("[BROADCAST] - Message to clients: %s\n", message);
    for (int i = 0; i < server->clientCount; ++i) {
        if (sfTcpSocket_send(server->clients[i], message, strlen(message)) != sfSocketDone) {
            printf("Failed to send message to client %d\n", i);
        }
    }
    pthread_mutex_unlock(&server->clientMutex);
}

void broadcast_grid(Server* server) {
    char message[256];
    for (int i = 0; i < server->currentGrid->rows * server->currentGrid->columns; i++) {
        Pexeso* card = server->currentGrid->pexesoObjects[i];
        snprintf(message, sizeof(message), "DATA %d %c %u\n", getID(card), getLabel(card), getIntegerBasedOnColor(*getColor(card)));
        broadcast_message(server, message);
    }
    broadcast_message(server, "COMPLETE\n");
}

void broadcastPairedCards(Server* server, int cardID1, int cardID2)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "PAIRED_CARDS %d %d\n", cardID1, cardID2);
    sfSleep(sfMilliseconds(1000));
    broadcast_message(server, buffer);
    printf("[SERVER] Broadcasting - pairs. %s", buffer);
}

void broadcastResetCards(Server* server, int cardID1, int cardID2){
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "HIDE_CARDS %d %d\n", cardID1, cardID2);
    sfSleep(sfMilliseconds(1000));
    broadcast_message(server, buffer);
    printf("[SERVER] Broadcasting - reset. %s", buffer);
}

bool checkPairedCards(Pexeso* pexFirst, Pexeso* pexSecond){
    bool ret = false;
    printf("Checking match..\n");
    printf("Pex[1] - %u \nPex[2] - %u\n---\n", getIntegerBasedOnColor(*getColor(pexFirst)),getIntegerBasedOnColor(*getColor(pexSecond)));
    printf("Pex[1] - %c \nPex[2] - %c\n", getLabel(pexFirst),getLabel(pexSecond));
    if(getIntegerBasedOnColor(*getColor(pexFirst)) == getIntegerBasedOnColor(*getColor(pexSecond)) && getLabel(pexFirst) == getLabel(pexSecond))
    {
        printf("Pexeso are matching!\n");
        ret = true;
    }
    return ret;
}

Pexeso* findByID(Server* server, int id) {
    for (int i = 0; i < server->currentGrid->rows * server->currentGrid->columns; i++) {
        Pexeso* card = server->currentGrid->pexesoObjects[i];
        if(id == getID(card))
        {
            printf("Found card by ID: %d == %d\n", id, getID(card));
            return card;
        }
    }
    printf("Could not find the card by ID.\n");
    return NULL;
}



void* handle_client(void* arg) {
    ClientHandlerArgs* args = (ClientHandlerArgs*)arg;
    Server* server = args->server;
    sfTcpSocket* client = args->client;

    free(args);

    printf("Client connected! Number of clients connected: %d\n", server->clientCount);
    char data[256];
    size_t received;
    if (sfTcpSocket_receive(client, data, sizeof(data) - 1, &received) != sfSocketDone) {
        printf("Failed to receive first message from client\n");
        sfTcpSocket_destroy(client);
        return NULL;
    }

    data[received] = '\0';
    printf("First message from client: %s\n", data);

    if (strncmp(data, "GRID", 4) == 0) {
        int rows, cols;
        sscanf(data, "GRID %d %d", &rows, &cols);
        char rowColMsg[128];
        snprintf(rowColMsg, sizeof(rowColMsg), "GRID %d %d\n", rows, cols);
        broadcast_message(server, rowColMsg);

        pthread_mutex_lock(&server->clientMutex);
        server->gridRows = rows;
        server->gridCols = cols;
        printf("Set grid dimensions to %dx%d from host\n", rows, cols);
        server->currentGrid = (PexesoGrid*)malloc(sizeof(PexesoGrid));
        server->currentGrid->rows = rows;
        server->currentGrid->columns = cols;
        server->currentGrid->pexesoObjects = (Pexeso**)malloc(sizeof(Pexeso*) * rows * cols);
        pthread_mutex_unlock(&server->clientMutex);
        if(!server->currentGrid){
            printf("Problem with generating grid on the server.\n");
        }
        printf("Grid generated on server\n");
        pexeso_grid_generate(server->currentGrid);
        broadcast_grid(server);
    }else {
        printf("Unexpected first message: %s\n", data);
    }

    while (1) {
        if (sfTcpSocket_receive(client, data, sizeof(data) - 1, &received) != sfSocketDone) {
            printf("Client disconnected or an error occurred\n");
            break;
        }

        data[received] = '\0';
        if (received > 0 && data[received - 1] == '\n') {
            data[received - 1] = '\0';
        }
        printf("[SERVER] Received from client: %s\n", data);
        if (strncmp(data, "GRID", 4) == 0) {
            int rows, cols;
            sscanf(data, "GRID %d %d\n", &rows, &cols);

            pthread_mutex_lock(&server->clientMutex);
            server->gridRows = rows;
            server->gridCols = cols;
            pthread_mutex_unlock(&server->clientMutex);

            printf("Updated grid dimensions to %dx%d\n", rows, cols);
            broadcast_message(server, data);
        } else if (strncmp(data, "START_GAME", 10) == 0) {
            broadcast_message(server, data);
        } else if (strncmp(data, "CARD_CLICK", 10) == 0) {
            int cardID;
            if (sscanf(data, "CARD_CLICK %d", &cardID) == 1) {
                printf("Received card click: ID=%d\n", cardID);

                size_t len = strlen(data);
                data[len] = '\n';
                broadcast_message(server, data);
                Pexeso* pex = findByID(server, cardID);
                if (server->pexesoToCompare == 0) {
                    server->revealedPexesoCards[0] = pex;
                    server->pexesoToCompare = 1;
                }else if (server->pexesoToCompare == 1) {
                    server->revealedPexesoCards[1] = pex;
                    bool matched = checkPairedCards(server->revealedPexesoCards[0],server->revealedPexesoCards[1]);
                    if (matched) {
                        printf("MATCH found on the server!\n");
                        broadcastPairedCards(server, getID(server->revealedPexesoCards[0]),
                                             getID(server->revealedPexesoCards[1]));
                    } else {
                        printf("Pexeso are not matching - resetting.\n");
                        broadcastResetCards(server, getID(server->revealedPexesoCards[0]),
                                            getID(server->revealedPexesoCards[1]));
                    }
                    server->pexesoToCompare = 0;
                    server->revealedPexesoCards[0] = NULL;
                    server->revealedPexesoCards[1] = NULL;

                }
            } else {
                printf("Invalid CARD message: %s\n", data);
            }

        } else if(strncmp(data, "PAIRED_CARDS", 12) == 0) {
            /*
            int cardID_1, cardID_2;
            if (sscanf(data,"PAIRED_CARDS %d %d", &cardID_1, &cardID_2) == 2) {
                printf("Received matching IDs ID_1=%d, ID_2=%d\n", cardID_1, cardID_2);
                broadcast_message(server, data);
            }
             */

        } else if (strncmp(data, "OK", 2) == 0) {
            printf("Acknowledgment received from client: %s\n", data);
        } else {
            printf("Error ? %s\n", data);
        }
    }

    // Cleanup the client
    pthread_mutex_lock(&server->clientMutex);
    for (int i = 0; i < server->clientCount; ++i) {
        if (server->clients[i] == client) {
            server->clients[i] = server->clients[--server->clientCount];
            break;
        }
    }
    pthread_mutex_unlock(&server->clientMutex);

    sfTcpSocket_destroy(client);
    return NULL;
}

int main() {
    Server server = { .clientCount = 0, .gridRows = 10, .gridCols = 10, .pexesoToCompare = 0 };
    pthread_mutex_init(&server.clientMutex, NULL);

    sfTcpListener* listener = sfTcpListener_create();
    if (!listener) {
        printf("Failed to create listener\n");
        return 1;
    }

    if (sfTcpListener_listen(listener, 53000, sfIpAddress_LocalHost) != sfSocketDone) {
        printf("Failed to bind listener\n");
        sfTcpListener_destroy(listener);
        return 1;
    }
    printf("Server is running..\n");

    while (1) {
        sfTcpSocket* client = NULL;

        if (sfTcpListener_accept(listener, &client) != sfSocketDone) {
            printf("Failed to accept a client\n");
            continue;
        }

        pthread_mutex_lock(&server.clientMutex);
        if (server.clientCount >= MAX_CLIENTS) {
            printf("Maximum client limit reached. Disconnecting client.\n");
            sfTcpSocket_destroy(client);
            pthread_mutex_unlock(&server.clientMutex);
            continue;
        }

        server.clients[server.clientCount++] = client;
        pthread_mutex_unlock(&server.clientMutex);

        ClientHandlerArgs* args = malloc(sizeof(ClientHandlerArgs));
        args->server = &server;
        args->client = client;

        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, args) != 0) {
            printf("Failed to create thread for client\n");
            pthread_mutex_lock(&server.clientMutex);
            --server.clientCount;
            pthread_mutex_unlock(&server.clientMutex);
            sfTcpSocket_destroy(client);
        } else {
            pthread_detach(client_thread);
            printf("Client thread created.\n");
        }
    }

    pthread_mutex_destroy(&server.clientMutex);
    sfTcpListener_destroy(listener);
    return 0;
}