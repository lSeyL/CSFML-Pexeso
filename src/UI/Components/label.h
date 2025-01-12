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

Label* labelCreate(const char* text, sfFont* font, sfVector2f position, unsigned int fontSize, sfColor color);
void labelSetText(Label* label, const char* text);
const char* labelGetText(Label* label);
void labelSetPosition(Label* label, sfVector2f position);
void labelDraw(Label* label, sfRenderWindow* window);
void labelDestroy(Label* label);
#endif //PEXESO_LABEL_H
