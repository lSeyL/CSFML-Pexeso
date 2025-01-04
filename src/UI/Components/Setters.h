//
// Created by seil0 on 3. 1. 2025.
//

#ifndef PEXESO_SETTERBUTTONS_H
#define PEXESO_SETTERBUTTONS_H
#include <SFML/Graphics.h>

typedef struct {
    sfRectangleShape** buttons;
    sfText** labels;
    size_t buttonCount;
    int selectedIndex;
} SetterButtons;

SetterButtons* setterCreateWithNumbers(const sfVector2f* startPosition, const sfVector2f* buttonSize, int count, const sfFont* font);
SetterButtons* setterCreateWithName(const sfVector2f* startPosition, const sfVector2f* buttonSize, const char** labels, size_t labelCount, const sfFont* font);
void drawSetters(SetterButtons* dimensionButtons, sfRenderWindow* window);
void setters_handleEvent(SetterButtons* dimensionButtons, const sfEvent* event);
void highlightButton(SetterButtons* dimensionButtons, size_t index);
int getSelected(const SetterButtons* dimensionButtons);
void settersDestroy(SetterButtons* dimensionButtons);
#endif //PEXESO_SETTERBUTTONS_H
