#pragma once
#include "defs.h"
#include "move.h"

enum TTFlag{
    TT_EXACT,
    TT_ALPHA,
    TT_BETA
};

struct TTEntry{
    Bitboard hashKey;
    int depth;
    int score;
    int flag;
    Move bestMove;
};

const int TT_SIZE = 1000000;

void initTT();
void clearTT();
void storeTT(Bitboard hashKey, int depth, int score, int flag, Move bestMove);
bool probeTT(Bitboard hashKey, int depth, int alpha, int beta, int& returnScore, Move& bestMove);

