#pragma once

#include "board.h"
#include "move.h"
#include "magic.h"
#include <vector>

extern Bitboard KnightMoves[64];
extern Bitboard bishopMoveTable[64][512];

void addPawnMove(std::vector<Move>& moveList, int from, int to, int flag, bool isWhite);
void generatePawnMoves(const Board& b, std::vector<Move>& moveList);
void initKnightMoveTable();
void generateKnightMoves(const Board& b, std::vector<Move>& moveList);
void initBishopMoveTable();
void generateBishopMoves(const Board& b, std::vector<Move>& moveList);