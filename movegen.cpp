#include "movegen.h"



void addPawnMove(std::vector<Move>& moveList, int from, int to, int flag, bool isWhite) {
    bool isPromotion = (isWhite && to > 55) || (!isWhite && to < 8);

    if (isPromotion) {
        // If the base flag was CAPTURE, we use the PROMO_CAPTURE versions
        int base = (flag == CAPTURE) ? 12 : 8; // Assumes your enum order
        moveList.push_back(createMove(from, to, base));     // Knight
        moveList.push_back(createMove(from, to, base + 1)); // Bishop
        moveList.push_back(createMove(from, to, base + 2)); // Rook
        moveList.push_back(createMove(from, to, base + 3)); // Queen
    } else {
        moveList.push_back(createMove(from, to, flag));
    }
}

void generatePawnMoves(const Board& b, std::vector<Move>& moveList){
    Bitboard pawns;
    Bitboard singlePush;
    Bitboard doublePush;
    Bitboard captureRight;
    Bitboard captureLeft;
    if(b.whiteMove){
        pawns = b.pieces[PAWN_W];
        singlePush = (pawns << 8) & ~b.allPieces;
        doublePush = (singlePush << 8) & ~b.allPieces & 0x00000000FF000000ULL;
        captureRight = (pawns << 9) & (b.blackPieces | (1ULL << b.enPassantSq)) & ~FILE_A;
        captureLeft = (pawns << 7) & (b.blackPieces | (1ULL << b.enPassantSq)) & ~FILE_H;
    }else{
        pawns = b.pieces[PAWN_B];
        singlePush = (pawns >> 8) & ~b.allPieces;
        doublePush = (singlePush >> 8) & ~b.allPieces & 0x000000FF00000000ULL;
        captureRight = (pawns >> 9) & (b.whitePieces | (1ULL << b.enPassantSq)) & ~FILE_H;
        captureLeft = (pawns >> 7) & (b.whitePieces | (1ULL << b.enPassantSq)) & ~FILE_A;
    }
    while(singlePush){
        //Find index of next 1 bit
        int toSq = popLSB(singlePush);
        int fromSq  = toSq + ((b.whiteMove) ? -8 : 8);
        addPawnMove(moveList, fromSq, toSq, QUIET, b.whiteMove);
    }
    while(doublePush){
        int toSq = popLSB(doublePush);
        int fromSq = toSq + ((b.whiteMove) ? -16 : 16);
        addPawnMove(moveList, fromSq, toSq, DOUBLE_PAWN_PUSH, b.whiteMove);
    }
    while(captureRight){
        int toSq = popLSB(captureRight);
        int fromSq = toSq + ((b.whiteMove) ? -9 : 9);
        int flag = (toSq == b.enPassantSq) ? EPCAPTURE : CAPTURE;
        addPawnMove(moveList, fromSq, toSq, flag, b.whiteMove);
    }
    while(captureLeft){
        int toSq = popLSB(captureLeft);
        int fromSq = toSq + ((b.whiteMove) ? -7 : 7);
        int flag = (toSq == b.enPassantSq) ? EPCAPTURE : CAPTURE;
        addPawnMove(moveList, fromSq, toSq, flag, b.whiteMove);
    }
}

Bitboard KnightMoves[64];

void initKnightMoveTable(){
    for(int sq = 0; sq < 64; sq++){
        Bitboard moves = 0ULL;
        Bitboard knight = (1ULL << sq);
        // Vertical 2, Horizontal 1
        if (!(knight & FILE_A)) moves |= (knight << 15) | (knight >> 17);
        if (!(knight & FILE_H)) moves |= (knight << 17) | (knight >> 15);
        
        // Vertical 1, Horizontal 2
        if (!(knight & (FILE_A | FILE_B))) moves |= (knight << 6) | (knight >> 10);
        if (!(knight & (FILE_G | FILE_H))) moves |= (knight << 10) | (knight >> 6);

        KnightMoves[sq] = moves;
    }

}

void generateKnightMoves(const Board& b, std::vector<Move>& moveList){
    Bitboard knights = (b.whiteMove) ? b.pieces[KNIGHT_W] : b.pieces[KNIGHT_B];
    Bitboard myPieces = (b.whiteMove) ? b.whitePieces : b.blackPieces;

    while(knights){
        int fromSq = popLSB(knights);

        Bitboard moves = KnightMoves[fromSq] & ~myPieces;
        while(moves){
            int toSq = popLSB(moves);
            int flag = ((1ULL << toSq) & b.allPieces) ? CAPTURE : QUIET;
            moveList.push_back(createMove(fromSq, toSq, flag));
        }
    }
}

Bitboard bishopMoveTable[64][512];

void initBishopMoveTable(){
    initBishopMask();
    for(int sq = 0; sq < 64; sq++){
        Bitboard mask = bishopMask[sq];
        int bits = std::popcount(mask);
        int combinations = 1 << bits;
        int shift = 64 - bits;

        for(int i = 0; i < combinations; i++){
            Bitboard blockers = setBlockers(i, bits, mask);
            int index = ((blockers * BISHOP_MAGICS[sq]) >> shift);
            Bitboard attacks = bishopAttacks(sq, blockers);

            bishopMoveTable[sq][index] = attacks;
        }
    }
}

void generateBishopMoves(const Board& b, std::vector<Move>& moveList){
    Bitboard bishops = (b.whiteMove) ? b.pieces[BISHOP_W] : b.pieces[BISHOP_B];
    Bitboard myPieces = (b.whiteMove) ? b.whitePieces : b.blackPieces;
    while(bishops){
        int fromSq = popLSB(bishops);
        Bitboard blockers = b.allPieces & bishopMask[fromSq];
        int bits = std::popcount(bishopMask[fromSq]);
        int shift = 64 - bits;
        int index = (blockers * BISHOP_MAGICS[fromSq]) >> shift;
        Bitboard moves = bishopMoveTable[fromSq][index] & ~myPieces;
        while(moves){
            int toSq = popLSB(moves);
            int flag = ((1ULL << toSq) & b.allPieces) ? CAPTURE : QUIET;
            moveList.push_back(createMove(fromSq, toSq, flag));
        }
    }
}

Bitboard rookMoveTable[64][4096];

void initRookMoveTable(){
    initRookMask();
    for(int sq = 0; sq < 64; sq++){
        Bitboard mask = rookMask[sq];
        int bits = std::popcount(mask);
        int combinations = 1 << bits;
        int shift = 64 - bits;

        for(int i = 0; i < combinations; i++){
            Bitboard blockers = setBlockers(i, bits, mask);
            int index = ((blockers * ROOK_MAGICS[sq]) >> shift);
            Bitboard attacks = rookAttacks(sq, blockers);

            rookMoveTable[sq][index] = attacks;
        }
    }
}

void generateRookMoves(const Board& b, std::vector<Move>& moveList){
    Bitboard rooks = (b.whiteMove) ? b.pieces[ROOK_W] : b.pieces[ROOK_B];
    Bitboard myPieces = (b.whiteMove) ? b.whitePieces : b.blackPieces;
    while(rooks){
        int fromSq = popLSB(rooks);
        Bitboard blockers = b.allPieces & rookMask[fromSq];
        int bits = std::popcount(rookMask[fromSq]);
        int shift = 64 - bits;
        int index = (blockers * ROOK_MAGICS[fromSq]) >> shift;
        Bitboard moves = rookMoveTable[fromSq][index] & ~myPieces;
        while(moves){
            int toSq = popLSB(moves);
            int flag = ((1ULL << toSq) & b.allPieces) ? CAPTURE : QUIET;
            moveList.push_back(createMove(fromSq, toSq, flag));
        }
    }
}

