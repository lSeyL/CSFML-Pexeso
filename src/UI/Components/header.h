//
// Created by seil0 on 2. 1. 2025.
//

#ifndef PEXESO_HEADER_H
#define PEXESO_HEADER_H
#include <SFML/Graphics.h>

typedef struct {
    sfSprite* sprite;
    sfTexture* texture;
} Header;;
Header* headerCreate(const char* texturePath, float x, float y);
void headerSetPos(Header* header, float x, float y);
void headerDestroy(Header* header);
#endif //PEXESO_HEADER_H
