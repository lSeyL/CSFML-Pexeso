//
// Created by seil0 on 3. 1. 2025.
//

#ifndef PEXESO_PEXESO_H
#define PEXESO_PEXESO_H
#include <SFML/Graphics.h>

typedef struct {
    sfRectangleShape* shape;
    sfText* textLabel;
    sfFont* font;
    sfColor backColor;
    sfColor frontColor;
    char label;
    int id;
    sfBool revealed;
    sfBool clicked;
} Pexeso;
Pexeso* pexesoCreate(const sfVector2f* position, const sfVector2f* size, sfColor backColor, sfColor frontColor, char label);
void pexesoDraw(Pexeso* pexeso, sfRenderWindow* window);
void handleEvent(Pexeso* pexeso, const sfEvent* event, sfRenderWindow* window);
sfBool isClicked(const Pexeso* pexeso, const sfVector2i* mousePosition);
void reveal(Pexeso* pexeso);
void hide(Pexeso* pexeso);
void setColor(Pexeso* pexeso, sfColor color);
sfFloatRect getBounds(const Pexeso* pexeso);
const sfColor* getColor(const Pexeso* pexeso);
char getLabel(const Pexeso* pexeso);
sfBool getRevealed(const Pexeso* pexeso);
int getID(const Pexeso* pexeso);
void setID(Pexeso* pexeso, int id);
void destroy(Pexeso* pexeso);
#endif //PEXESO_PEXESO_H
