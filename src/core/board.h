#pragma once
#include "defs.h"
#include "move.h"

typedef struct UndoInfo{
	int movingPiece;
	int capturedPiece;
	int enPassantSq;
	int castlingRights;
	int halfMoveClock;
	bool whiteMove;
}UndoInfo;

typedef struct Board{
	Bitboard pieces[12];//0-5 are white pieces(P, N, B, R, Q, K) 6 - 11 are black pieces

	Bitboard whitePieces;
	Bitboard blackPieces;
	Bitboard allPieces;
	bool whiteMove;
	int enPassantSq;
	int castlingRights;
	int halfMoveClock;
}Board;

Board generateBoard();

void printBitboard(Bitboard bb);

UndoInfo makeMove(Board &b, Move m);
void unMakeMove(Board &b, Move m, UndoInfo info);
