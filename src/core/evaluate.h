#pragma once
#include "board.h"

const int PAWN_VALUE = 100;
const int KNIGHT_VALUE = 300;
const int BISHOP_VALUE = 320;
const int ROOK_VALUE = 500;
const int QUEEN_VALUE = 900;

int evaluate(Board &b);