#pragma once
#include "board.h"
#include "evaluate.h"
#include <vector>

const int INF = 50000;
const int MATE_SCORE = 49000;

int negamax(Board &b, int depth, int alpha, int beta);
Move searchBestMove(Board &b, int depth);