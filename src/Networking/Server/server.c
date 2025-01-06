//
// Created by seil0 on 1. 1. 2025.
//

#include "server.h"

void broadcast_message(Server* server, const char* message) {
    pthread_mutex_lock(&server->clientMutex);
    for (int i = 0; i < server->clientCount; ++i) {

        if (sfTcpSocket_send(server->clients[i], message, strlen(message)) != sfSocketDone) {
            printf("Failed to send message to client %d\n", i);
        } else {
            printf("[BROADCAST] - Message to clients: %s", message);
        }
    }
    pthread_mutex_unlock(&server->clientMutex);
}

void* handle_client(void* arg) {
    ClientHandlerArgs* args = (ClientHandlerArgs*)arg;
    Server* server = args->server;
    sfTcpSocket* client = args->client;

    free(args);

    printf("Client connected! Number of clients connected: %d", server->clientCount);

    // Wait for the first message from the client
    char data[256];
    size_t received;
    if (sfTcpSocket_receive(client, data, sizeof(data) - 1, &received) != sfSocketDone) {
        printf("Failed to receive first message from client\n");
        sfTcpSocket_destroy(client);
        return NULL;
    }

    data[received] = '\0'; // Null-terminate the received data
    printf("First message from client: %s\n", data);

    if (strncmp(data, "GRID", 4) == 0) {
        int rows, cols;
        sscanf(data, "GRID %d %d", &rows, &cols);

        pthread_mutex_lock(&server->clientMutex);
        server->gridRows = rows;
        server->gridCols = cols;
        pthread_mutex_unlock(&server->clientMutex);

        printf("Set grid dimensions to %dx%d from host\n", rows, cols);

        broadcast_message(server, data);


        pthread_mutex_lock(&server->clientMutex);
        //send_full_grid_state(server, client);
        pthread_mutex_unlock(&server->clientMutex);
    } else {
        printf("Unexpected first message: %s\n", data);
    }

    // Handle subsequent messages from the client
    while (1) {
        if (sfTcpSocket_receive(client, data, sizeof(data) - 1, &received) != sfSocketDone) {
            printf("Client disconnected or an error occurred\n");
            break;
        }

        data[received] = '\0';
        printf("Received from client: %s\n", data);

        // Process messages as usual
        if (strncmp(data, "GRID", 4) == 0) {
            int rows, cols;
            sscanf(data, "GRID %d %d", &rows, &cols);

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
                broadcast_message(server, data);
            } else {
                printf("Invalid CARD message: %s\n", data);
            }

        } else if(strncmp(data, "PAIRED_CARDS", 12) == 0) {
            int cardID_1, cardID_2;
            if (sscanf(data,"PAIRED_CARDS %d %d", &cardID_1, &cardID_2) == 2) {
                printf("Received matching IDs ID_1=%d, ID_2=%d\n", cardID_1, cardID_2);
                broadcast_message(server, data);
            }
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
    Server server = { .clientCount = 0, .gridRows = 10, .gridCols = 10 };
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