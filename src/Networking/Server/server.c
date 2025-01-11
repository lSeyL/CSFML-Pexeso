//
// Created by seil0 on 1. 1. 2025.
//

#include "server.h"

void broadcast_message(Server *server, const char *message) {
    pthread_mutex_lock(&server->clientMutex);
    printf("[BROADCAST] - Message to clients: %s\n", message);
    for (int i = 0; i < server->clientCount; ++i) {
        if(server->clients[i]) {
        if (sfTcpSocket_send(server->clients[i], message, strlen(message)) != sfSocketDone) {
            printf("Failed to send message to client %d\n", i);
        }
        }
    }
    pthread_mutex_unlock(&server->clientMutex);
}

void broadcast_clientID(Server *server, ClientArg* args) {
    pthread_mutex_lock(&server->clientMutex);
    for (int i = 0; i < server->clientCount; ++i) {
        char buffer[64];
        args->id = i;
        printf("Assigning id to client: %d\n",args->id );
        snprintf(buffer, sizeof(buffer), "CLIENT %d\n", i);
        if (sfTcpSocket_send(server->clients[i], buffer, strlen(buffer)) != sfSocketDone) {
            printf("Failed to send message to client %d\n", i);
        }
    }
    pthread_mutex_unlock(&server->clientMutex);
}

void broadcast_grid(Server *server) {
    char message[256];
    for (int i = 0; i < server->currentGrid->rows * server->currentGrid->columns; i++) {
        Pexeso *card = server->currentGrid->pexesoObjects[i];
        snprintf(message, sizeof(message), "DATA %d %c %u\n", getID(card), getLabel(card),
                 getIntegerBasedOnColor(*getColor(card)));
        broadcast_message(server, message);
    }
    broadcast_message(server, "COMPLETE\n");
}

void broadcastPairedCards(Server *server, int cardID1, int cardID2) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "PAIRED_CARDS %d %d\n", cardID1, cardID2);
    sfSleep(sfMilliseconds(1000));
    broadcast_message(server, buffer);
    printf("[SERVER] Broadcasting - pairs. %s", buffer);
}

void broadcastResetCards(Server *server, int cardID1, int cardID2) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "HIDE_CARDS %d %d\n", cardID1, cardID2);
    sfSleep(sfMilliseconds(1000));
    broadcast_message(server, buffer);
    printf("[SERVER] Broadcasting - reset. %s", buffer);
}


void nextTurn(Server *server) {

    int current = server->currentClientTurn;
    server->currentClientTurn = (server->currentClientTurn + 1) % server->clientCount;
    printf("Changing turn from: %d -> to -> %d \n", current, server->currentClientTurn );
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "TURN %d\n", server->currentClientTurn);
    printf("[SERVER]Sending TURN to client with ID %d\n", server->currentClientTurn);
    broadcast_message(server, buffer);
}

void addPointsToCurrentClient(Server *server, bool addPoints) {
    pthread_mutex_lock(&server->clientMutex);
    int currentClient = server->currentClientTurn;
    bool canAddPoints = false;
    if (addPoints) {
        if (server->revealedPexesoCards[0] && server->revealedPexesoCards[1]) {
            if (!server->revealedPexesoCards[0]->wasFound && !server->revealedPexesoCards[1]->wasFound) {
                canAddPoints = true;
                server->revealedPexesoCards[0]->wasFound = sfTrue;
                server->revealedPexesoCards[1]->wasFound = sfTrue;
            }
        }
    }
    if (canAddPoints) {
        server->points[currentClient] += 100;
    }
    int currentPoints = server->points[currentClient];
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "UPDATE %d\n", currentPoints);
    if (sfTcpSocket_send(server->clients[currentClient], buffer, strlen(buffer)) != sfSocketDone) {
    }
    printf("[SERVER] Broadcasting - update points. %s", buffer);
    pthread_mutex_unlock(&server->clientMutex);
}

bool checkPairedCards(Pexeso *pexFirst, Pexeso *pexSecond) {
    bool ret = false;
    if (getIntegerBasedOnColor(*getColor(pexFirst)) == getIntegerBasedOnColor(*getColor(pexSecond)) &&
        getLabel(pexFirst) == getLabel(pexSecond)) {
        printf("Pexeso are matching!\n");
        if (pexFirst == pexSecond) return false;
        ret = true;
    }
    return ret;
}

void checkGameState(Server *server) {
    server->isGameFinished = true;
    int totalCards = server->currentGrid->rows * server->currentGrid->columns;
    for (int i = 0; i < totalCards; i++) {
        Pexeso *card = server->currentGrid->pexesoObjects[i];
        if (!card->wasFound) {
            server->isGameFinished = false;
            return;
        }
    }
}

Pexeso *findByID(Server *server, int id) {
    for (int i = 0; i < server->currentGrid->rows * server->currentGrid->columns; i++) {
        Pexeso *card = server->currentGrid->pexesoObjects[i];
        if (id == getID(card)) {
            //printf("Found card by ID: %d == %d\n", id, getID(card));
            return card;
        }
    }
    printf("Could not find the card by ID.\n");
    return NULL;
}

void *handle_client(void *arg) {
    ClientArg *args = (ClientArg *) arg;
    Server *server = args->server;
    sfTcpSocket *client = args->client;
    srand(time(NULL));
    server->isGameFinished = false;

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
        broadcast_clientID(server, args);
        char randTurnMsg[8];
        int randomStartTurn = rand() % server->clientCount;
        server->currentClientTurn = randomStartTurn;
        snprintf(randTurnMsg, sizeof(randTurnMsg), "TURN %d\n", randomStartTurn);
        broadcast_message(server, randTurnMsg);
        pthread_mutex_lock(&server->clientMutex);
        server->gridRows = rows;
        server->gridCols = cols;
        printf("Set grid dimensions to %dx%d from host\n", rows, cols);
        server->currentGrid = (PexesoGrid *) malloc(sizeof(PexesoGrid));
        server->currentGrid->rows = rows;
        server->currentGrid->columns = cols;
        server->currentGrid->pexesoObjects = (Pexeso **) malloc(sizeof(Pexeso *) * rows * cols);
        pthread_mutex_unlock(&server->clientMutex);
        if (!server->currentGrid) {
            printf("Problem with generating grid on the server.\n");
        }
        printf("Grid generated on server\n");
        pexeso_grid_generate(server->currentGrid);
        broadcast_grid(server);
        server->isGameRunning = true;
        sfTcpSocket_setBlocking(client, sfFalse);
    } else {
        printf("Unexpected first message: %s\n", data);
    }

    while (1) {
        sfTime elapsed = sfClock_getElapsedTime(server->pingClock);
        if (sfTime_asSeconds(elapsed) >= 10.0f) {
            printf("Sending ping to client...\n");
            const char *pingMessage = "PING\n";
            broadcast_message(server, pingMessage);
            sfClock_restart(server->pingClock);
        }

        sfSocketStatus status = sfTcpSocket_receive(client, data, sizeof(data) - 1, &received);
        if (status == sfSocketDisconnected) {
            printf("Client disconnected or an error occurred\n");
            int currentClientId = args->id;
            bool wasClientsTurn = false;
            pthread_mutex_lock(&server->clientMutex);
            if (server->clients[server->currentClientTurn] == client) {
                printf("Disconnected client was on turn. Moving to the next turn.\n");
                wasClientsTurn = true;
            } else {
                printf("Disconnected client was not on turn.\n");
            }
            pthread_mutex_unlock(&server->clientMutex);
            pthread_mutex_lock(&server->clientMutex);
            int saveClientCount = server->clientCount;
            for (int i = 0; i < server->clientCount; ++i) {
                if (server->clients[i] == client) {
                    printf("Marking client ID %d as disconnected.\n", i);
                    ClientArg* disconnectedClient = (ClientArg*)server->clients[i];
                    disconnectedClient->id = -1;
                    server->clientCount--;
                    break;
                }
            }
            pthread_mutex_unlock(&server->clientMutex);
            int activeClientsNum = 0;
            pthread_mutex_lock(&server->clientMutex);
            for (int i = 0; i < saveClientCount; ++i) {
                ClientArg* clientArg = (ClientArg*)server->clients[i];
                if (clientArg->id == -1) {
                    activeClientsNum++;
                }
            }
            pthread_mutex_unlock(&server->clientMutex);
            printf("Active clients: %d", activeClientsNum);

            if (wasClientsTurn) {
                if (activeClientsNum > 0) {
                    if(activeClientsNum == 1) {

                        int nextClientId = -1;
                        pthread_mutex_lock(&server->clientMutex);
                        for (int i = 0; i < saveClientCount; ++i) {
                            ClientArg* clientArg = (ClientArg*)server->clients[i];
                            if (clientArg->id != -1) {
                                nextClientId = i;
                                break;
                            }
                        }
                        pthread_mutex_unlock(&server->clientMutex);
                        server->currentClientTurn = nextClientId;
                        printf("Only one client left.[Turn %d] -> Turn goes to client %d.\n", currentClientId, server->currentClientTurn);
                    } else {
                        int curr = server->currentClientTurn;
                        server->currentClientTurn %= server->clientCount;
                        printf("Changing client turn from: %d -> to -> %d\n", curr, server->currentClientTurn);
                        nextTurn(server);
                    }

                } else {
                    printf("No clients left. Ending game.\n");
                    server->isGameRunning = false;
                    server->isGameFinished = true;
                }

            }
            printf("Crash 1\n");
            pthread_mutex_unlock(&server->clientMutex);
            if (args) {
                free(args);
                args = NULL;
            } else {
                printf("args is NULL. Skipping free.\n");
            }
            printf("Crash 2\n");
            pthread_mutex_lock(&server->clientMutex);
            if (client) {
                printf("Destroying client socket: %p\n", (void*)client);
                for (int i = 0; i < server->clientCount; ++i) {
                    if (server->clients[i] == client) {
                        server->clients[i] = NULL;
                        break;
                    }
                }
                //sfTcpSocket_destroy(client);
                client = NULL;
            } else {
                printf("client is NULL. Skipping destroy.\n");
            }
            pthread_mutex_unlock(&server->clientMutex);
            printf("Crash 3\n");
            printf("Socket for disconnected client destroyed.\n");
            break;
        } else if (status == sfSocketNotReady) {
            sfSleep(sfMilliseconds(100));
            continue;
        } else if (status == sfSocketDone) {

            if (!server->isGameRunning && server->isGameFinished && server->clientCount) {
                return NULL;
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
                    Pexeso *pex = findByID(server, cardID);
                    if (server->pexesoToCompare == 0) {
                        pthread_mutex_lock(&server->clientMutex);
                        server->revealedPexesoCards[0] = pex;
                        server->pexesoToCompare = 1;
                        pthread_mutex_unlock(&server->clientMutex);
                    } else if (server->pexesoToCompare == 1) {
                        pthread_mutex_lock(&server->clientMutex);
                        server->revealedPexesoCards[1] = pex;
                        pthread_mutex_unlock(&server->clientMutex);
                        bool matched = checkPairedCards(server->revealedPexesoCards[0], server->revealedPexesoCards[1]);
                        if (matched) {
                            printf("MATCH found on the server!\n");
                            broadcastPairedCards(server, getID(server->revealedPexesoCards[0]),
                                                 getID(server->revealedPexesoCards[1]));
                            addPointsToCurrentClient(server, true);

                        } else {
                            printf("Pexeso are not matching - resetting.\n");
                            broadcastResetCards(server, getID(server->revealedPexesoCards[0]),
                                                getID(server->revealedPexesoCards[1]));
                            nextTurn(server);
                            addPointsToCurrentClient(server, false);
                        }
                        pthread_mutex_lock(&server->clientMutex);
                        server->pexesoToCompare = 0;
                        server->revealedPexesoCards[0] = NULL;
                        server->revealedPexesoCards[1] = NULL;
                        pthread_mutex_unlock(&server->clientMutex);
                        checkGameState(server);
                        if (server->isGameRunning) {
                            printf("[SERVER]Game is running.\n");
                            if (server->isGameFinished) {
                                printf("[SERVER]Game finished.\n");
                                char winMsg[5];
                                snprintf(winMsg, sizeof(winMsg), "WIN\n");
                                broadcast_message(server, winMsg);
                                server->isGameRunning = false;
                                server->isGameFinished = true;
                            }
                        }

                    }

                } else {
                    printf("Invalid CARD message: %s\n", data);
                }
            } else if (strncmp(data, "PAIRED_CARDS", 12) == 0) {
                //
            } else if (strncmp(data, "OK", 2) == 0) {
                printf("Acknowledgment received from client: %s\n", data);
            } else if (strncmp(data, "PONG", 4) == 0) {
                printf("Received PONG from client.\n");

            } else {
                printf("Error ? %s\n", data);
            }
        }
        /*
        if(args) {
            free(args);
        }
         */

    }

    sfTcpSocket_destroy(client);
    printf("Socket for disconnected client destroyed.\n");
    return NULL;
}

int main() {
    Server server = {.clientCount = 0, .gridRows = 10, .gridCols = 10, .pexesoToCompare = 0, .isGameFinished = false, .isGameRunning = false};
    for (int i = 0; i < MAX_CLIENTS; i++) {
        server.points[i] = 0;
    }
    server.pingClock = sfClock_create();
    server.gameClock = sfClock_create();

    pthread_mutex_init(&server.clientMutex, NULL);
    sfTcpListener *listener = sfTcpListener_create();
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
        sfTcpSocket *client = NULL;

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

        ClientArg *args = malloc(sizeof(ClientArg));
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
        pthread_mutex_lock(&server.clientMutex);
        if (!server.isGameRunning && server.clientCount == 0) {
            free(args);
            break;
        }
        pthread_mutex_unlock(&server.clientMutex);
    }
    sfClock_destroy(server.pingClock);
    sfClock_destroy(server.gameClock);
    pthread_mutex_destroy(&server.clientMutex);
    sfTcpListener_destroy(listener);
    return 0;
}