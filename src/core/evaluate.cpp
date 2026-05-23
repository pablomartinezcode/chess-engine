#include "evaluate.h"
#include <bit>

int evaluate(Board& b) {
    int score = 0;

    score += std::popcount(b.pieces[PAWN_W]) * PAWN_VALUE;
    score += std::popcount(b.pieces[KNIGHT_W]) * KNIGHT_VALUE;
    score += std::popcount(b.pieces[BISHOP_W]) * BISHOP_VALUE;
    score += std::popcount(b.pieces[ROOK_W]) * ROOK_VALUE;
    score += std::popcount(b.pieces[QUEEN_W]) * QUEEN_VALUE;

    score -= std::popcount(b.pieces[PAWN_B]) * PAWN_VALUE;
    score -= std::popcount(b.pieces[KNIGHT_B]) * KNIGHT_VALUE;
    score -= std::popcount(b.pieces[BISHOP_B]) * BISHOP_VALUE;
    score -= std::popcount(b.pieces[ROOK_B]) * ROOK_VALUE;
    score -= std::popcount(b.pieces[QUEEN_B]) * QUEEN_VALUE;

    return score;
}

int getPieceValue(int piece) {
    if (piece == PAWN_W || piece == PAWN_B) return PAWN_VALUE;
    if (piece == KNIGHT_W || piece == KNIGHT_B) return KNIGHT_VALUE;
    if (piece == BISHOP_W || piece == BISHOP_B) return BISHOP_VALUE;
    if (piece == ROOK_W || piece == ROOK_B) return ROOK_VALUE;
    if (piece == QUEEN_W || piece == QUEEN_B) return QUEEN_VALUE;
    return 0; //King or invalid piece
}