//
// Created by seil0 on 1. 1. 2025.
//

#include <SFML/Network.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
void* handle_client(void* client_socket) {
    sfTcpSocket* client = (sfTcpSocket*)client_socket;

    printf("Client connected!\n");

    // Loop to continuously receive data from the client
    while (1) {
        char data[100];
        size_t received;

        // Receive data from the client
        if (sfTcpSocket_receive(client, data, sizeof(data), &received) != sfSocketDone) {
            printf("Client disconnected or an error occurred\n");
            break;
        }

        // Null-terminate the received data
        data[received] = '\0';
        printf("Received from client: %s\n", data);

        // Echo the data back to the client
        if (sfTcpSocket_send(client, data, received) != sfSocketDone) {
            printf("Failed to send data back to the client\n");
            break;
        }
    }

    // Cleanup and exit the thread
    sfTcpSocket_destroy(client);
    return NULL;
}

int main()
{
    // Create a TCP listener
    sfTcpListener* listener = sfTcpListener_create();
    if (!listener) {
        printf("Failed to create listener\n");
        return 1;
    }

    // Bind the listener to a port and listen on all addresses
    if (sfTcpListener_listen(listener, 53000, sfIpAddress_Any) != sfSocketDone) {
        printf("Failed to bind listener\n");
        sfTcpListener_destroy(listener);
        return 1;
    }
    printf("Server is listening on port 53000...\n");

    // Loop to accept multiple clients
    while (1) {
        sfTcpSocket* client = NULL;

        // Accept a new client
        if (sfTcpListener_accept(listener, &client) != sfSocketDone) {
            printf("Failed to accept a client\n");
            continue;
        }

        // Create a new thread to handle the client
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, client) != 0) {
            printf("Failed to create thread for client\n");
            sfTcpSocket_destroy(client);
        } else {
            // Detach the thread to allow independent execution
            pthread_detach(client_thread);
        }
    }

    // Cleanup (never reached in this example)
    sfTcpListener_destroy(listener);
    return 0;
}