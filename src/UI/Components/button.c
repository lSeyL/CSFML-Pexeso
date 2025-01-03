//
// Created by seil0 on 2. 1. 2025.
//
#include "button.h"
#include <stdlib.h>
#include <stdio.h>

Button* buttonCreate(const char* texturePath, float x, float y) {
    Button* button = (Button*)malloc(sizeof(Button));
    if (!button) return NULL;

    button->texture = sfTexture_createFromFile(texturePath, NULL);
    if (!button->texture) {
        printf("Failed to load texture: %s\n", texturePath);
        free(button);
        return NULL;
    }

    sfVector2u textureSize = sfTexture_getSize(button->texture);
    button->shape = sfRectangleShape_create();
    sfRectangleShape_setTexture(button->shape, button->texture, sfTrue);
    sfRectangleShape_setPosition(button->shape, (sfVector2f){x, y});
    sfRectangleShape_setSize(button->shape, (sfVector2f){textureSize.x, textureSize.y});
    button->hitbox = sfRectangleShape_getGlobalBounds(button->shape);
    return button;
}
sfBool buttonClicked(Button* button, sfVector2f mousePos) {
    if (!button) return sfFalse;
    button->hitbox = sfRectangleShape_getGlobalBounds(button->shape);
    return sfFloatRect_contains(&button->hitbox, mousePos.x, mousePos.y);
}
void buttonDestroy(Button* button) {
    if (button) {
        if (button->texture) sfTexture_destroy(button->texture);
        if (button->shape) sfRectangleShape_destroy(button->shape);
        free(button);
    }
}