//
// Created by seil0 on 3. 1. 2025.
//
#include "label.h"
#include <stdlib.h>
#include <stdio.h>

Label* label_create(const char* text, sfFont* font, sfVector2f position, unsigned int fontSize, sfColor color) {
    if (!font) {
        printf("Error: Font is NULL\n");
        return NULL;
    }

    Label* label = (Label*)malloc(sizeof(Label));
    if (!label) return NULL;

    label->text = sfText_create();
    if (!label->text) {
        printf("Failed to create text object\n");
        free(label);
        return NULL;
    }

    sfText_setFont(label->text, font);
    sfText_setString(label->text, text);
    sfText_setCharacterSize(label->text, fontSize);
    sfText_setFillColor(label->text, color);
    sfText_setPosition(label->text, position);

    label->position = position;

    return label;
}

void label_set_text(Label* label, const char* text) {
    if (label && label->text) {
        sfText_setString(label->text, text);
    }
}

void label_set_position(Label* label, sfVector2f position) {
    if (label && label->text) {
        sfText_setPosition(label->text, position);
        label->position = position;
    }
}

void label_draw(Label* label, sfRenderWindow* window) {
    if (label && label->text) {
        sfRenderWindow_drawText(window, label->text, NULL);
    }
}

void label_destroy(Label* label) {
    if (label) {
        if (label->text) sfText_destroy(label->text);
        free(label);
    }
}