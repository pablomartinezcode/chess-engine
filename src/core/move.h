#pragma once
#include "defs.h"
#include <string>
#include <vector>

struct Board;

struct Move {       //FLAGS   TO    FROM
    uint16_t data;  // 0000 000000 000000

    int getFrom() const {return data & 0x3F;}//First 6 Bits
    int getTo() const {return (data >> 6) & 0x3F;}//Next 6 bits
    int getFlags() const {return (data >> 12) & 0x0F;}//Last 4 bits
};

enum MoveFlags {
    QUIET = 0,
    DOUBLE_PAWN_PUSH = 1,
    KING_CASTLE = 2,
    QUEEN_CASTLE = 3,
    CAPTURE = 4,
    EPCAPTURE = 5,
    // Promotion Flags
    PROMOTION_N = 8,
    PROMOTION_B = 9,
    PROMOTION_R = 10,
    PROMOTION_Q = 11,
    // Promotion + Capture
    PROMO_CAPTURE_N = 12,
    PROMO_CAPTURE_B = 13,
    PROMO_CAPTURE_R = 14,
    PROMO_CAPTURE_Q = 15
};

Move createMove(int from, int to, int flags);
int parseSquare(std::string sq);
Move parseMove(std::string moveStr, Board &b);