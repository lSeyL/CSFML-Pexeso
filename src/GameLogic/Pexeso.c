//
// Created by seil0 on 3. 1. 2025.
//
#include "Pexeso.h"


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
    pexeso->wasFound = sfFalse;
    pexeso->id = -1;
    pexeso->shape = sfRectangleShape_create();
    sfRectangleShape_setPosition(pexeso->shape, *position);
    sfRectangleShape_setSize(pexeso->shape, *size);
    sfRectangleShape_setFillColor(pexeso->shape, backColor);
    // "../../Resources/Roboto-Light.ttf"
    // "C:/pos_sem/Pexeso/Resources/Roboto-Light.ttf"
    const char* fontPath = "../../Resources/Roboto-Light.ttf";
    pexeso->font = sfFont_createFromFile(fontPath);
    if (!pexeso->font) {
        printf("Pexeso - Failed to load font: %s\n", fontPath);
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
void handleClickOnCard(Pexeso* pexeso, const sfEvent* event) {
    if (!pexeso || !event) return;
    if (event->type == sfEvtMouseButtonPressed) {
        sfVector2f mousePos = {event->mouseButton.x, event->mouseButton.y};
        if (isClicked(pexeso, &mousePos)) {
            if (!pexeso->revealed && !pexeso->wasFound) {
                reveal(pexeso);
                //printf("ID: %d revealed.\n", getID(pexeso));
            }
        }
    }
}
sfBool isClicked(const Pexeso* pexeso, const sfVector2f* mousePosition) {
    if (!pexeso || !mousePosition) return sfFalse;
    if(pexeso->wasFound) return sfFalse;
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
void setWasFound(Pexeso* pexeso) {
    pexeso->wasFound = sfTrue;
    sfRectangleShape_setFillColor(pexeso->shape, sfTransparent);
}