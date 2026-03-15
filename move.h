#pragma once
#include "defs.h"
#include <string>

struct Move {       //FLAGS   TO    FROM
    uint16_t data;  // 0000 000000 000000

    int getFrom() const {return data & 0x3F;}//First 6 Bits
    int getTo() const {return (data >> 6) & 0x3F;}//Next 6 bits
    int getFlags() const {return (data >> 12) & 0x0F;}//Last 4 bits
};

Move createMove(int from, int to, int flags);
int parseSquare(std::string sq);
Move parseMove(std::string moveStr);