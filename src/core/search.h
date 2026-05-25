#pragma once
#include "board.h"
#include "evaluate.h"
#include <vector>
#include <chrono>

const int INF = 50000;
const int MATE_SCORE = 49000;


int negamax(Board &b, int depth, int alpha, int beta, int ply = 0);
Move searchBestMove(Board &b, int depth);
Move searchBestMoveTimed(Board &b, std::chrono::milliseconds moveTime, int maxDepth = 64);
