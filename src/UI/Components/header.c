//
// Created by seil0 on 2. 1. 2025.
//
#include "header.h"
#include <stdlib.h>
#include <stdio.h>

Header* headerCreate(const char* texturePath, float x, float y) {
    Header* header = (Header*)malloc(sizeof(Header));
    if (!header) return NULL;
    header->texture = sfTexture_createFromFile(texturePath, NULL);
    if (!header->texture) {
        printf("Failed to load texture: %s\n", texturePath);
        free(header);
        return NULL;
    }
    header->sprite = sfSprite_create();
    sfSprite_setTexture(header->sprite, header->texture, sfTrue);
    sfSprite_setPosition(header->sprite, (sfVector2f){x, y});

    return header;
}
void headerSetPos(Header* header, float x, float y) {
    if (header && header->sprite) {
        sfSprite_setPosition(header->sprite, (sfVector2f){x, y});
    }
}
void headerDestroy(Header* header) {
    if (header) {
        if (header->sprite) sfSprite_destroy(header->sprite);
        if (header->texture) sfTexture_destroy(header->texture);
        free(header);
    }
}