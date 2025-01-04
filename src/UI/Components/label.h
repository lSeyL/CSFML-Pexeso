//
// Created by seil0 on 3. 1. 2025.
//

#ifndef PEXESO_LABEL_H
#define PEXESO_LABEL_H
#include <SFML/Graphics.h>

typedef struct {
    sfText* text;
    sfVector2f position;
} Label;

Label* label_create(const char* text, sfFont* font, sfVector2f position, unsigned int fontSize, sfColor color);
void label_set_text(Label* label, const char* text);
void label_set_position(Label* label, sfVector2f position);
void label_draw(Label* label, sfRenderWindow* window);
void label_destroy(Label* label);
#endif //PEXESO_LABEL_H
