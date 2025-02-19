//
// Created by seil0 on 3. 1. 2025.
//
#include "Setters.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
SetterButtons* setterCreateWithNumbers(const sfVector2f* startPosition, const sfVector2f* buttonSize, int count, const sfFont* font) {
    SetterButtons* dimensionButtons = (SetterButtons*)malloc(sizeof(SetterButtons));
    if (!dimensionButtons) return NULL;
    dimensionButtons->buttons = (sfRectangleShape**)malloc(sizeof(sfRectangleShape*) * count);
    dimensionButtons->labels = (sfText**)malloc(sizeof(sfText*) * count);
    dimensionButtons->buttonCount = count;
    dimensionButtons->selectedIndex = -1;
    for (int i = 0; i < count; ++i) {
        sfVector2f position = {
                startPosition->x + i * (buttonSize->x + 10),
                startPosition->y
        };
        sfRectangleShape* button = sfRectangleShape_create();
        sfRectangleShape_setPosition(button, position);
        sfRectangleShape_setSize(button, *buttonSize);
        sfRectangleShape_setFillColor(button, sfColor_fromRGB(43, 43, 43));
        dimensionButtons->buttons[i] = button;

        sfText* label = sfText_create();
        char labelStr[10];
        snprintf(labelStr, sizeof(labelStr), "%d", i + 2);
        sfText_setString(label, labelStr);
        sfText_setFont(label, font);
        sfText_setCharacterSize(label, 20);
        sfVector2f labelPosition = { position.x + 10, position.y + 5 };
        sfText_setPosition(label, labelPosition);
        dimensionButtons->labels[i] = label;
    }

    return dimensionButtons;
}

SetterButtons* setterCreateWithName(const sfVector2f* startPosition, const sfVector2f* buttonSize, const char** labels, size_t labelCount, const sfFont* font) {
    SetterButtons* dimensionButtons = (SetterButtons*)malloc(sizeof(SetterButtons));
    if (!dimensionButtons) return NULL;

    dimensionButtons->buttons = (sfRectangleShape**)malloc(sizeof(sfRectangleShape*) * labelCount);
    dimensionButtons->labels = (sfText**)malloc(sizeof(sfText*) * labelCount);
    dimensionButtons->buttonCount = labelCount;
    dimensionButtons->selectedIndex = -1;

    if (!dimensionButtons->buttons || !dimensionButtons->labels) {
        settersDestroy(dimensionButtons);
        return NULL;
    }

    for (size_t i = 0; i < dimensionButtons->buttonCount; ++i) {
        dimensionButtons->buttons[i] = NULL;
        dimensionButtons->labels[i] = NULL;
    }

    for (size_t i = 0; i < labelCount; ++i) {
        sfVector2f position = {
                startPosition->x + i * (buttonSize->x + 10),
                startPosition->y
        };

        sfRectangleShape* button = sfRectangleShape_create();
        sfRectangleShape_setPosition(button, position);
        sfRectangleShape_setSize(button, *buttonSize);
        sfRectangleShape_setFillColor(button, sfColor_fromRGB(43, 43, 43));
        dimensionButtons->buttons[i] = button;
        sfText* label = sfText_create();
        sfText_setString(label, labels[i]);
        sfText_setFont(label, font);
        sfText_setCharacterSize(label, 20);
        sfVector2f labelPosition = { position.x + 10, position.y + 5 };
        sfText_setPosition(label, labelPosition);
        dimensionButtons->labels[i] = label;
    }
    return dimensionButtons;
}

void drawSetters(SetterButtons* dimensionButtons, sfRenderWindow* window) {
    for (size_t i = 0; i < dimensionButtons->buttonCount; ++i) {
        sfRenderWindow_drawRectangleShape(window, dimensionButtons->buttons[i], NULL);
        sfRenderWindow_drawText(window, dimensionButtons->labels[i], NULL);
    }
}
void setters_handleEvent(SetterButtons* dimensionButtons, const sfEvent* event) {
    if (!dimensionButtons || !event) return;

    if (event->type == sfEvtMouseButtonPressed) {
        sfVector2f mousePosF = {event->mouseButton.x, event->mouseButton.y};
        for (size_t i = 0; i < dimensionButtons->buttonCount; ++i) {
            sfFloatRect bounds = sfRectangleShape_getGlobalBounds(dimensionButtons->buttons[i]);
            if (sfFloatRect_contains(&bounds, mousePosF.x, mousePosF.y)) {
                dimensionButtons->selectedIndex = i;
                highlightButton(dimensionButtons, i);
                break;
            }
        }
    }
}

int getSelected(const SetterButtons* dimensionButtons) {
    return dimensionButtons->selectedIndex + 2;
}

int getSelectedBase(const SetterButtons* dimensionButtons) {
    return dimensionButtons->selectedIndex + 1;
}
void setDefaultSelectedIndex(SetterButtons* dimensionButtons, size_t index)
{
    dimensionButtons->selectedIndex = index;
}
void highlightButton(SetterButtons* dimensionButtons, size_t index) {
    for (size_t i = 0; i < dimensionButtons->buttonCount; ++i) {
        if (i == index) {
            sfRectangleShape_setFillColor(dimensionButtons->buttons[i], sfColor_fromRGB(19, 99, 208));
        } else {
            sfRectangleShape_setFillColor(dimensionButtons->buttons[i], sfColor_fromRGB(43, 43, 43));
        }
    }
}
void settersDestroy(SetterButtons* dimensionButtons) {
    if (!dimensionButtons) return;
    for (size_t i = 0; i < dimensionButtons->buttonCount; ++i) {
        if (dimensionButtons->buttons[i]) {
            sfRectangleShape_destroy(dimensionButtons->buttons[i]);
            dimensionButtons->buttons[i] = NULL;
        }
        if (dimensionButtons->labels[i]) {
            sfText_destroy(dimensionButtons->labels[i]);
            dimensionButtons->labels[i] = NULL;
        }
    }
    if (dimensionButtons->buttons) {
        free(dimensionButtons->buttons);
        dimensionButtons->buttons = NULL;
    }
    if (dimensionButtons->labels) {
        free(dimensionButtons->labels);
        dimensionButtons->labels = NULL;
    }
    free(dimensionButtons);
    printf("SetterButtons destroyed.\n");
}