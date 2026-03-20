#pragma once
#include <cstdint>
#include <bit>

typedef uint64_t Bitboard;
enum { WHITE, BLACK };
enum { PAWN_W, KNIGHT_W, BISHOP_W, ROOK_W, QUEEN_W, KING_W, PAWN_B, KNIGHT_B, BISHOP_B, ROOK_B, QUEEN_B, KING_B };
enum Castling {WK = 1, WQ = 2, BK = 4, BQ = 8};

enum Squares {
    a1 = 0, b1, c1, d1, e1, f1, g1, h1,
    a2 = 8, b2, c2, d2, e2, f2, g2, h2,
    a3 = 16, b3, c3, d3, e3, f3, g3, h3,
    a4 = 24, b4, c4, d4, e4, f4, g4, h4,
    a5 = 32, b5, c5, d5, e5, f5, g5, h5,
    a6 = 40, b6, c6, d6, e6, f6, g6, h6,
    a7 = 48, b7, c7, d7, e7, f7, g7, h7,
    a8 = 56, b8, c8, d8, e8, f8, g8, h8,
};

const Bitboard FILE_A = 0x0101010101010101ULL;
const Bitboard FILE_B = FILE_A << 1;
const Bitboard FILE_C = FILE_B << 1;
const Bitboard FILE_D = FILE_C << 1;
const Bitboard FILE_E = FILE_D << 1;
const Bitboard FILE_F = FILE_E << 1;
const Bitboard FILE_G = FILE_F << 1;
const Bitboard FILE_H = FILE_G << 1;

inline constexpr Bitboard FILES[8] = {
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H
};

const Bitboard RANK_1 = 0x00000000000000FFULL;
const Bitboard RANK_2 = 0x000000000000FF00ULL;
const Bitboard RANK_3 = 0x0000000000FF0000ULL;
const Bitboard RANK_4 = 0x00000000FF000000ULL;
const Bitboard RANK_5 = 0x000000FF00000000ULL;
const Bitboard RANK_6 = 0x0000FF0000000000ULL;
const Bitboard RANK_7 = 0x00FF000000000000ULL;
const Bitboard RANK_8 = 0xFF00000000000000ULL;

inline constexpr Bitboard RANKS[8] = {
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8
};

const Bitboard PERIMETER = RANK_1 | RANK_8 | FILE_A | FILE_H;

inline int popLSB(Bitboard& b){
    int sq = std::countr_zero(b);
    b &= (b - 1);
    return sq;
}

