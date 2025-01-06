//
// Created by seil0 on 3. 1. 2025.
//
#include "Rules.h"


Rules* rules_create() {
    Rules* rules = (Rules*)malloc(sizeof(Rules));
    if (!rules) return NULL;

    rules->isPaired = false;
    rules->difficulty = 1;
    rules->mode = 0; // 0 - unlimited, 1 - time-limited
    rules->isBotGame = false;
    return rules;
}
void rules_destroy(Rules* rules) {
    if (rules) {
        free(rules);
    }
}
bool checkPair(Rules* rules, int rows, int columns) {
    if (!rules) return false;
    if ((rows * columns) % 2 != 0) {
        rules->isPaired = false;
    } else {
        rules->isPaired = true;
    }

    return rules->isPaired;
}
bool getIsPaired(const Rules* rules) {
    if (!rules) return false;
    return rules->isPaired;
}
void setDifficulty(Rules* rules, int difficulty) {
    if (!rules) return;
    rules->difficulty = difficulty;
}
void setMode(Rules* rules, int mode) {
    if (!rules) return;
    rules->mode = mode;
}
int GetDifficulty(const Rules* rules) {
    if (!rules) return -1;
    return rules->difficulty;
}
int getMode(const Rules* rules) {
    if (!rules) return -1;
    return rules->mode;
}