//
// Created by seil0 on 3. 1. 2025.
//
#include "Pexeso.h"
#include <stdlib.h>
#include <stdio.h>

unsigned int getIntegerBasedOnColor(sfColor color) {
    return (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
}
Pexeso* pexesoCreate(const sfVector2f* position, const sfVector2f* size, sfColor backColor, sfColor frontColor, char label) {
    Pexeso* pexeso = (Pexeso*)malloc(sizeof(Pexeso));
    if (!pexeso) return NULL;
    pexeso->backColor = backColor;
    pexeso->frontColor = frontColor;
    pexeso->label = label;
    pexeso->revealed = sfFalse;
    pexeso->clicked = sfFalse;
    pexeso->id = -1;
    pexeso->shape = sfRectangleShape_create();
    sfRectangleShape_setPosition(pexeso->shape, *position);
    sfRectangleShape_setSize(pexeso->shape, *size);
    sfRectangleShape_setFillColor(pexeso->shape, backColor);
    const char* fontPath = "../../Resources/Roboto-Light.ttf";
    pexeso->font = sfFont_createFromFile(fontPath);
    if (!pexeso->font) {
        printf("Failed to load font: %s\n", fontPath);
        free(pexeso);
        return NULL;
    }
    pexeso->textLabel = sfText_create();
    sfText_setString(pexeso->textLabel, (char[]){label, '\0'});
    sfText_setFont(pexeso->textLabel, pexeso->font);
    sfText_setCharacterSize(pexeso->textLabel, 42);
    sfText_setFillColor(pexeso->textLabel, sfWhite);
    sfText_setStyle(pexeso->textLabel, sfTextBold);
    sfFloatRect textBounds = sfText_getLocalBounds(pexeso->textLabel);
    sfText_setOrigin(pexeso->textLabel, (sfVector2f){textBounds.width / 2, textBounds.height / 2});
    sfText_setPosition(pexeso->textLabel, (sfVector2f){
            position->x + size->x / 2,
            position->y + size->y / 2
    });
    return pexeso;
}
void pexesoDraw(Pexeso* pexeso, sfRenderWindow* window) {
    sfRenderWindow_drawRectangleShape(window, pexeso->shape, NULL);
    if (pexeso->revealed) {
        sfRenderWindow_drawText(window, pexeso->textLabel, NULL);
    }
}
void handleEvent(Pexeso* pexeso, const sfEvent* event, sfRenderWindow* window) {
    if (event->type == sfEvtMouseButtonPressed) {
        sfVector2i mousePos = sfMouse_getPositionRenderWindow(window);
        if (isClicked(pexeso, &mousePos)) {
            printf("Click: ID:[%d], COLOR:[%u], LABEL:[%c]\n",
                   pexeso->id,
                   getIntegerBasedOnColor(pexeso->frontColor),
                   pexeso->label);
            if (!pexeso->revealed) {
                reveal(pexeso);
            }
        }
    }
}
sfBool isClicked(const Pexeso* pexeso, const sfVector2i* mousePosition) {
    sfFloatRect bounds = sfRectangleShape_getGlobalBounds(pexeso->shape);
    return sfFloatRect_contains(&bounds, mousePosition->x, mousePosition->y);
}
void reveal(Pexeso* pexeso) {
    pexeso->revealed = sfTrue;
    sfRectangleShape_setFillColor(pexeso->shape, pexeso->frontColor);
}
void hide(Pexeso* pexeso) {
    pexeso->revealed = sfFalse;
    sfRectangleShape_setFillColor(pexeso->shape, pexeso->backColor);
}
void setColor(Pexeso* pexeso, sfColor color) {
    sfRectangleShape_setFillColor(pexeso->shape, color);
}
sfFloatRect getBounds(const Pexeso* pexeso) {
    return sfRectangleShape_getGlobalBounds(pexeso->shape);
}
char getLabel(const Pexeso* pexeso) {
    return pexeso->label;
}
sfBool getRevealed(const Pexeso* pexeso) {
    return pexeso->revealed;
}
const sfColor* getColor(const Pexeso* pexeso) {
    return &pexeso->frontColor;
}
int getID(const Pexeso* pexeso) {
    return pexeso->id;
}
void setID(Pexeso* pexeso, int id) {
    pexeso->id = id;
}
void destroy(Pexeso* pexeso) {
    if (pexeso) {
        if (pexeso->shape) sfRectangleShape_destroy(pexeso->shape);
        if (pexeso->textLabel) sfText_destroy(pexeso->textLabel);
        if (pexeso->font) sfFont_destroy(pexeso->font);
        free(pexeso);
    }
}