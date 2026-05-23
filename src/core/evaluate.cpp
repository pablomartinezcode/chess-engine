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
