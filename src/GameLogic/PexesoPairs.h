//
// Created by seil0 on 3. 1. 2025.
//

#ifndef PEXESO_PEXESOPAIRS_H
#define PEXESO_PEXESOPAIRS_H
#include <SFML/Graphics.h>

typedef struct {
    sfColor color;
    char label;
} PexesoPair;

PexesoPair* pexeso_pairs_generate(int rows, int columns);
void shuffle_pairs(PexesoPair* pairs, int count);
#endif //PEXESO_PEXESOPAIRS_H
