#pragma once
#include "defs.h"
#include <random>
#include <iostream>


extern Bitboard bishopMask[64];
extern const Bitboard BISHOP_MAGICS[64];

extern Bitboard rookMask[64];
extern const Bitboard ROOK_MAGICS[64];

void initBishopMask();
Bitboard setBlockers(int index, int bits, Bitboard mask);
Bitboard bishopAttacks(int sq, Bitboard blockers);
int buildBishopAttackData(int sq, Bitboard blockers[512], Bitboard attacks[512]);
bool testBishopMagicNumber(int sq, Bitboard magic, const Bitboard blockers[512], const Bitboard attacks[512], int possibleCombinations);
Bitboard randomU64();
Bitboard randomU64FewBits();
Bitboard findBishopMagicForSquare(int sq);
void findAllBishopMagicSquares();

void initRookMask();
Bitboard rookAttacks(int sq, Bitboard blockers);
int buildRookAttackData(int sq, Bitboard blockers[4096], Bitboard attacks[4096]);
bool testRookMagicNumber(int sq, Bitboard magic, const Bitboard blockers[4096], const Bitboard attacks[4096], int possibleCombinations);
Bitboard findRookMagicForSquare(int sq);
void findAllRookMagicSquares();
