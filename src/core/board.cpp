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



UndoInfo makeMove(Board &b, Move m){
    UndoInfo undo;
    undo.movingPiece = -1;
    undo.capturedPiece = -1;
    undo.enPassantSq = b.enPassantSq;
    undo.castlingRights = b.castlingRights;
    undo.halfMoveClock = b.halfMoveClock;
    undo.whiteMove = b.whiteMove;

    int from = m.getFrom();
    int to = m.getTo();
    int flags = m.getFlags();
    int piece = -1;

    //1. FIND MOVING PIECE
    const int start = (b.whiteMove) ? 0 : 6;
    const int end = start + 6;
    
    for(int i = start; i < end; i++){
        //Pushes bits in b.pieces[i] exactly "from" bits will bring forward 
        //the selected square to first bit, where we can then & with 1ULL
        if((b.pieces[i] >> from) & 1ULL){
            piece = i;
            break;
        }
    }
    undo.movingPiece = piece;
    if(piece == -1) return undo; //Invalid Move

    b.halfMoveClock++; //Increment half-move clock (reset later if pawn move or capture)
    if(piece == PAWN_W || piece == PAWN_B || flags == CAPTURE || flags == EPCAPTURE || (flags >= PROMO_CAPTURE_N && flags <= PROMO_CAPTURE_Q)) b.halfMoveClock = 0; //Reset half-move clock if pawn move
    
    //RESET enPassantSq at the start of every move
    b.enPassantSq = -1;


    if(flags == DOUBLE_PAWN_PUSH){
        b.enPassantSq = (b.whiteMove) ? (from + 8) : (from - 8);
    }

    if(flags == EPCAPTURE){
        int capSq = (b.whiteMove) ? (to - 8) : (to + 8);
        int capPiece = (b.whiteMove) ? PAWN_B : PAWN_W;
        b.pieces[capPiece] ^= (1ULL << capSq);
        undo.capturedPiece = capPiece;
        b.halfMoveClock = 0; //Reset half-move clock if capture
    }
    
    //2. REMOVE PIECE FROM ITS ORIGIN
    b.pieces[piece] ^= (1ULL << from);

    //Only place on to square if not a promotion
    if(flags < PROMOTION_N) b.pieces[piece] ^= (1ULL << to);

    if(flags >= PROMOTION_N && flags <= PROMOTION_Q){
        int promoPiece = (b.whiteMove ? PAWN_W : PAWN_B) + (flags - PROMOTION_N) + 1;
        b.pieces[promoPiece] |= (1ULL << to);
    }

    if(flags >= PROMO_CAPTURE_N && flags <= PROMO_CAPTURE_Q){
        int promoPiece = (b.whiteMove ? PAWN_W : PAWN_B) + (flags - PROMO_CAPTURE_N) + 1;
        b.pieces[promoPiece] |= (1ULL << to);
    }

    if(flags == KING_CASTLE){
        if(b.whiteMove){ 
            b.pieces[ROOK_W] ^= (1ULL << h1) | (1ULL << f1);
        }else{
            b.pieces[ROOK_B] ^= (1ULL << h8) | (1ULL << f8);
        }
    }else if(flags == QUEEN_CASTLE){
        if(b.whiteMove){ 
            b.pieces[ROOK_W] ^= (1ULL << a1) | (1ULL << d1);
        }else{
            b.pieces[ROOK_B] ^= (1ULL << a8) | (1ULL << d8);
        }
    }

    //3. HANDLE CAPTURES AND REMOVE FROM BOARD
    if(flags != EPCAPTURE){
        int oppStart = (b.whiteMove) ? 6 : 0;
        int oppEnd = oppStart + 6;

        for(int i = oppStart; i < oppEnd; i++){
            if((b.pieces[i] >> to) & 1ULL){
                b.pieces[i] ^= (1ULL << to);
                undo.capturedPiece = i;
                b.halfMoveClock = 0; //Reset half-move clock if capture
                break;
            }
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

    // If King moves, remove all rights for that side
    if(piece == KING_W) b.castlingRights &= ~(WK | WQ);
    if(piece == KING_B) b.castlingRights &= ~(BK | BQ);

    // If a Rook moves (or is captured), remove that specific right
    if (from == a1 || to == a1) b.castlingRights &= ~WQ;
    if (from == h1 || to == h1) b.castlingRights &= ~WK;
    if (from == a8 || to == a8) b.castlingRights &= ~BQ;
    if (from == h8 || to == h8) b.castlingRights &= ~BK;
    
    return undo;
}

void unMakeMove(Board &b, Move m, UndoInfo undo){
    int from = m.getFrom();
    int to = m.getTo();
    int flags = m.getFlags();

    b.whiteMove = undo.whiteMove;
    b.enPassantSq = undo.enPassantSq;
    b.castlingRights = undo.castlingRights;
    b.halfMoveClock = undo.halfMoveClock;
    int piece = undo.movingPiece;

    //1. Undo Castling
    if(flags == KING_CASTLE){
        if(b.whiteMove){ 
            b.pieces[ROOK_W] ^= (1ULL << h1) | (1ULL << f1);
        }else{
            b.pieces[ROOK_B] ^= (1ULL << h8) | (1ULL << f8);
        }
    }else if(flags == QUEEN_CASTLE){
        if(b.whiteMove){ 
            b.pieces[ROOK_W] ^= (1ULL << a1) | (1ULL << d1);
        }else{
            b.pieces[ROOK_B] ^= (1ULL << a8) | (1ULL << d8);
        }
    }

    //2. Move piece back to original square
    if(flags < PROMOTION_N){
        b.pieces[piece] ^= (1ULL << to);
    }

    if(flags >= PROMOTION_N && flags <= PROMOTION_Q){
        int promoPiece = (b.whiteMove ? PAWN_W : PAWN_B) + (flags - PROMOTION_N) + 1;
        b.pieces[promoPiece] ^= (1ULL << to);
    }
    if(flags >= PROMO_CAPTURE_N && flags <= PROMO_CAPTURE_Q){
        int promoPiece = (b.whiteMove ? PAWN_W : PAWN_B) + (flags - PROMO_CAPTURE_N) + 1;
        b.pieces[promoPiece] ^= (1ULL << to);
    }

    b.pieces[piece] ^= (1ULL << from);

    //3. Restore captured piece if there was one
    if(flags == EPCAPTURE){
        int capSq = (b.whiteMove) ? (to - 8) : (to + 8);
        if(undo.capturedPiece != -1){
            b.pieces[undo.capturedPiece] ^= (1ULL << capSq);
        }
    }else{
        if(undo.capturedPiece != -1){
            b.pieces[undo.capturedPiece] ^= (1ULL << to);
        }
    }

    //4. Update composite bitboards
    b.whitePieces = 0ULL;
    for (int i = 0; i < 6; i++) b.whitePieces |= b.pieces[i];
    b.blackPieces = 0ULL;
    for (int i = 6; i < 12; i++) b.blackPieces |= b.pieces[i];
    b.allPieces = b.whitePieces | b.blackPieces;
}