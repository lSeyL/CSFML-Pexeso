#include <stdio.h>
#include "SFML/Graphics.h"
#include "SFML/Window.h"
#include <stdlib.h>
int main() {
    printf("Hello, World!\n");
    sfVideoMode mode = {800, 600, 32};
    sfRenderWindow* window;
    window = sfRenderWindow_create(mode, "SFML window", sfResize | sfClose, NULL);
    sfEvent event;
    while (sfRenderWindow_isOpen(window)) {
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
        }
    }

    sfRenderWindow_destroy(window);
    return EXIT_SUCCESS;
}
