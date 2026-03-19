#pragma once
#include "defs.h"
#include <bit>
#include <random>
#include <iostream>


extern Bitboard bishopMask[64];
extern  const Bitboard BISHOP_MAGICS[64];


void initBishopMask();
int popLSB(Bitboard& bb);
Bitboard setBishopBlockers(int index, int bits, Bitboard mask);
Bitboard bishopAttacks(int sq, Bitboard blockers);
int buildBishopAttackData(int sq, Bitboard blockers[512], Bitboard attacks[512]);
bool testBishopMagicNumber(int sq, Bitboard magic, const Bitboard blockers[512], const Bitboard attacks[512], int possibleCombinations);
Bitboard randomU64();
Bitboard randomU64FewBits();
Bitboard findBishopMagicForSquare(int sq);
void findAllMagicSquares();