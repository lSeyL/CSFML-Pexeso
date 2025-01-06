//
// Created by seil0 on 3. 1. 2025.
//

#ifndef PEXESO_RULES_H
#define PEXESO_RULES_H
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    bool isPaired;
    int difficulty;
    int mode;
    bool isBotGame;
} Rules;
Rules* rules_create();
void rules_destroy(Rules* rules);
bool checkPair(Rules* rules,int rows, int columns);
bool getIsPaired(const Rules* rules);
void setDifficulty(Rules* rules, int difficulty);
void setMode(Rules* rules, int mode);
int GetDifficulty(const Rules* rules);
int getMode(const Rules* rules);
#endif //PEXESO_RULES_H
