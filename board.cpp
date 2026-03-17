#include "board.h"
#include <iostream>

Board generateBoard(){
	Board b;
	for(int i = 0; i < 12; i++) b.pieces[i] = 0ULL;
	//Populate Piece Bitboards using Hex Constants
    // White Pieces (0-5)
    b.pieces[0] = 0x000000000000FF00ULL; // White Pawns (Rank 2)
    b.pieces[1] = 0x0000000000000042ULL; // White Knights (b1, g1)
    b.pieces[2] = 0x0000000000000024ULL; // White Bishops (c1, f1)
    b.pieces[3] = 0x0000000000000081ULL; // White Rooks (a1, h1)
    b.pieces[4] = 0x0000000000000008ULL; // White Queen (d1)
    b.pieces[5] = 0x0000000000000010ULL; // White King (e1)

    // Black Pieces (6-11)
    b.pieces[6] = 0x00FF000000000000ULL; // Black Pawns (Rank 7)
    b.pieces[7] = 0x4200000000000000ULL; // Black Knights (b8, g8)
    b.pieces[8] = 0x2400000000000000ULL; // Black Bishops (c8, f8)
    b.pieces[9] = 0x8100000000000000ULL; // Black Rooks (a8, h8)
    b.pieces[10] = 0x0800000000000000ULL; // Black Queen (d8)
    b.pieces[11] = 0x1000000000000000ULL; // Black King (e8)
	
	b.whitePieces = 0x000000000000FFFFULL;
	b.blackPieces = 0xFFFF000000000000ULL;
	b.allPieces = b.whitePieces | b.blackPieces;

	b.whiteMove = true; //White starts
	b.enPassantSq = -1; //Set en passant to no at start
	b.halfMoveClock = 0; //Reset 50-move rule clock

	b.castlingRights = 15; //15 = 1111 in binary. 1's represent rights available(WK | WQ | BK | BQ)
	return b;
}

void printBitboard(Bitboard bb) {
    std::cout << "  +-----------------+\n";
    
    // Loop through ranks from 8 down to 1
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << (rank + 1) << " | "; // Print rank number
        
        // Loop through files from a to h
        for (int file = 0; file < 8; file++) {
            // Calculate the square index (0 to 63)
            int square = rank * 8 + file;
            
            // Check if the bit at 'square' is set
            if ((bb >> square) & 1ULL) {
                std::cout << "X ";
            } else {
                std::cout << ". ";
            }
        }
        std::cout << "|\n";
    }
    
    std::cout << "  +-----------------+\n";
    std::cout << "    a b c d e f g h\n\n";
}

void makeMove(Board &b, Move m){
    int from = m.getFrom();
    int to = m.getTo();
    int flags = m.getFlags();
    int piece = -1;

    //RESET enPassantSq at the start of every move
    b.enPassantSq = -1;

    if(flags == DOUBLE_PAWN_PUSH){
        b.enPassantSq = (b.whiteMove) ? (from + 8) : (from - 8);
    }


    //1. FIND MOVING PIECE
    int start = (b.whiteMove) ? 0 : 6;
    int end = start + 6;
    
    for(int i = start; i < end; i++){
        //Pushes bits in b.pieces[i] exactly "from" bits will bring forward 
        //the selected square to first bit, where we can then & with 1ULL
        if((b.pieces[i] >> from) & 1ULL){
            piece = i;
            break;
        }
    }

    if(piece == -1) return; //Invalid Move

    //2. REMOVE PIECE FROM ITS ORIGIN
    //   AND PLACE AT DESTINATION
    b.pieces[piece] ^= (1ULL << from);
    b.pieces[piece] ^= (1ULL << to);


    //3. HANDLE CAPTURES AND REMOVE FROM BOARD
    int oppStart = (b.whiteMove) ? 6 : 0;
    int oppEnd = oppStart + 6;

    for(int i = oppStart; i < oppEnd; i++){
        if((b.pieces[i] >> to) & 1ULL){
            b.pieces[i] ^= (1ULL << to);
            break;
        }
    }


    //4. UPDATE COMPOSITE BITBOARDS
    b.whitePieces = 0ULL;
    for (int i = 0; i < 6; i++) b.whitePieces |= b.pieces[i];

    b.blackPieces = 0ULL;
    for (int i = 6; i < 12; i++) b.blackPieces |= b.pieces[i];

    b.allPieces = b.whitePieces | b.blackPieces;


    //5. SWITCH SIDES
    b.whiteMove = !b.whiteMove;

};