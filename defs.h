#pragma once
#include <cstdint>

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
const Bitboard FILE_H = 0x8080808080808080ULL;

const Bitboard FILE_B = 0x0202020202020202ULL;
const Bitboard FILE_G = 0x4040404040404040ULL;

