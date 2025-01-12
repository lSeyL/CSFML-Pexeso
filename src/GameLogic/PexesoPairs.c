//
// Created by seil0 on 3. 1. 2025.
//
#include "PexesoPairs.h"


PexesoPair* pexesoPairsGenerate(int rows, int columns) {
    int totalCards = rows * columns;
    if (totalCards % 2 != 0) {
        printf("Invalid grid size: Rows * Columns must be even.\n");
        return NULL;
    }

    int pairCount = totalCards / 2;
    PexesoPair* pairs = (PexesoPair*)malloc(sizeof(PexesoPair) * totalCards);

    if (!pairs) {
        printf("Failed to allocate memory for pairs\n");
        return NULL;
    }

    sfColor colors[] = {sfColor_fromRGB(214, 56, 56), sfColor_fromRGB(45, 97, 186),
                        sfColor_fromRGB(78, 173, 52), sfColor_fromRGB(194, 178, 58),
                        sfColor_fromRGB(71, 179, 154), sfColor_fromRGB(138, 54, 191),
                        sfColor_fromRGB(217, 131, 46), sfColor_fromRGB(224, 63, 125),
                        sfColor_fromRGB(46, 242, 145), sfColor_fromRGB(184, 100, 55)};
    char labels[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};

    int colorCount = sizeof(colors) / sizeof(colors[0]);
    int labelCount = sizeof(labels) / sizeof(labels[0]);

    if (pairCount > colorCount * labelCount) {
        printf("Error: Not enough pairs?\n");
        free(pairs);
        return NULL;
    }

    int pairIndex = 0;
    for (int i = 0; i < colorCount && pairIndex < pairCount; ++i) {
        for (int j = 0; j < labelCount && pairIndex < pairCount; ++j) {
            pairs[pairIndex * 2] = (PexesoPair){colors[j], labels[i]};
            pairs[pairIndex * 2 + 1] = (PexesoPair){colors[j], labels[i]};
            pairIndex++;
        }
    }

    shufflePairs(pairs, totalCards);

    return pairs;
}

void shufflePairs(PexesoPair* pairs, int count) {
    srand(time(NULL));
    for (int i = count - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        PexesoPair temp = pairs[i];
        pairs[i] = pairs[j];
        pairs[j] = temp;
    }
}