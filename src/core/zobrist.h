#pragma once
#include "board.h"
#include "defs.h"

extern Bitboard pieceKeys[12][64]; //Random keys for each piece on each square
extern Bitboard enPassantKeys[64]; //Random keys for en passant files
extern Bitboard castlingKeys[16]; //Random keys for castling rights (16 possible combinations)
extern Bitboard sideKey; //Random key for side to move

void initZobrist();

Bitboard generateHash(const Board &b);