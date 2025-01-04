//
// Created by seil0 on 31. 12. 2024.
//

#ifndef PEXESO_BUTTON_H
#define PEXESO_BUTTON_H
#include <SFML/Graphics.h>

typedef struct {
    sfRectangleShape* shape;
    sfTexture* texture;
    sfFloatRect hitbox;
} Button;

Button* buttonCreate(const char* texturePath, float x, float y);
sfBool buttonClicked(Button* button, sfEvent* event);
void buttonDestroy(Button* button);

#endif //PEXESO_BUTTON_H
