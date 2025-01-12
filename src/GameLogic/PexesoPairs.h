//
// Created by seil0 on 3. 1. 2025.
//

#ifndef PEXESO_PEXESOPAIRS_H
#define PEXESO_PEXESOPAIRS_H
#include <SFML/Graphics.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
typedef struct {
    sfColor color;
    char label;
} PexesoPair;

PexesoPair* pexesoPairsGenerate(int rows, int columns);
void shufflePairs(PexesoPair* pairs, int count);
#endif //PEXESO_PEXESOPAIRS_H
