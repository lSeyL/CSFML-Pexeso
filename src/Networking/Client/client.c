//
// Created by seil0 on 1. 1. 2025.
//
#include <stdio.h>
#include "../../UI/window.h"
int main()
{
    Window* window = window_create();
    if (!window) {
        printf("Failed to create window\n");
        return 1;
    }
    windowStart(window);
    windowDestroy(window);
    window = NULL;
    return 0;
}