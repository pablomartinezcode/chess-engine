#pragma once

#include "board.h"
#include "move.h"
#include <vector>

extern Bitboard KnightMoves[64];

void addPawnMove(std::vector<Move>& moveList, int from, int to, int flag, bool isWhite);
void generatePawnMoves(const Board& b, std::vector<Move>& moveList);
void initMoveTable();
void generateKnightMoves(const Board& b, std::vector<Move>& moveList);
//void generateBishopMoves(const Board& b, std::vector<Move>& moveList);
