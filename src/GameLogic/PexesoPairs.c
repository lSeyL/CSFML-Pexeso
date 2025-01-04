//
// Created by seil0 on 3. 1. 2025.
//
#include "PexesoPairs.h"
#include <stdlib.h>
#include <time.h>

PexesoPair* pexeso_pairs_generate(int rows, int columns) {
    int pairCount = (rows * columns) / 2;
    PexesoPair* pairs = (PexesoPair*)malloc(sizeof(PexesoPair) * rows * columns);

    sfColor colors[] = {sfRed, sfBlue, sfGreen, sfYellow, sfCyan, sfMagenta};
    char labels[] = {'A', 'B', 'C', 'D', 'E', 'F'};

    for (int i = 0; i < pairCount; ++i) {
        pairs[i * 2] = (PexesoPair){colors[i % 6], labels[i % 6]};
        pairs[i * 2 + 1] = (PexesoPair){colors[i % 6], labels[i % 6]};
    }

    return pairs;
}

void shuffle_pairs(PexesoPair* pairs, int count) {
    srand(time(NULL));
    for (int i = count - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        PexesoPair temp = pairs[i];
        pairs[i] = pairs[j];
        pairs[j] = temp;
    }
}