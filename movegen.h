#pragma once

#include "board.h"
#include "move.h"
#include "magic.h"
#include "defs.h"
#include <vector>

//Global for testing not really supposed to be
//extern Bitboard KnightMoves[64];
//extern Bitboard bishopMoveTable[64][512];
extern Bitboard kingMoveTable[64];

void addPawnMove(std::vector<Move>& moveList, int from, int to, int flag, bool isWhite);
void generatePawnMoves(const Board& b, std::vector<Move>& moveList);
void initKnightMoveTable();
void generateKnightMoves(const Board& b, std::vector<Move>& moveList);
void initBishopMoveTable();
void generateBishopMoves(const Board& b, std::vector<Move>& moveList);
void initRookMoveTable();
void generateRookMoves(const Board& b, std::vector<Move>& moveList);
void generateQueenMoves(const Board& b, std::vector<Move>& moveList);
void initKingMoveTable();
void generateKingMoves(const Board& b, std::vector<Move>& moveList);
bool isSquareAttacked(const Board& b, int sq, int attackerSide);
std::vector<Move> generateLegalMoves(Board &b);
