//
// Created by seil0 on 1. 1. 2025.
//
#include <SFML/Network.h>
#include <stdio.h>
#include <string.h>
#include "../../UI/window.h"
int main()
{
    /*
    // Create a TCP socket
    sfTcpSocket* socket = sfTcpSocket_create();
    if (!socket) {
        printf("Failed to create socket\n");
        return 1;
    }

    // Convert IP address string to sfIpAddress
    sfIpAddress serverAddress = sfIpAddress_fromString("127.0.0.1");

    // Connect to the server
    if (sfTcpSocket_connect(socket, serverAddress, 53000, sfSeconds(5)) != sfSocketDone) {
        printf("Failed to connect to server\n");
        sfTcpSocket_destroy(socket);
        return 1;
    }
    printf("Connected to server!\n");

    char message[100];
    while (1) {
        // Input message to send to the server
        printf("Enter a message: ");
        fgets(message, sizeof(message), stdin);

        // Remove newline character
        message[strcspn(message, "\n")] = '\0';

        // Send the message to the server
        if (sfTcpSocket_send(socket, message, strlen(message)) != sfSocketDone) {
            printf("Failed to send message\n");
            break;
        }

        // Receive a response from the server
        char response[100];
        size_t received;
        if (sfTcpSocket_receive(socket, response, sizeof(response), &received) != sfSocketDone) {
            printf("Failed to receive response\n");
            break;
        }

        // Null-terminate the received data and print it
        response[received] = '\0';
        printf("Server response: %s\n", response);
    }

    // Cleanup
    sfTcpSocket_destroy(socket);

    return 0;
     */
    Window* window = window_create();
    if (!window) {
        printf("Failed to create window\n");
        return 1;
    }

    windowStart(window);

    // Cleanup
    windowDestroy(window);

    return 0;
}