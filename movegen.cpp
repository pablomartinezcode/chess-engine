#include "movegen.h"
#include <bit>

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
        int toSq = std::countr_zero(singlePush);
        int fromSq  = toSq + ((b.whiteMove) ? -8 : 8);
        addPawnMove(moveList, fromSq, toSq, QUIET, b.whiteMove);
        singlePush &= (singlePush - 1);
    }
    while(doublePush){
        int toSq = std::countr_zero(doublePush);
        int fromSq = toSq + ((b.whiteMove) ? -16 : 16);
        addPawnMove(moveList, fromSq, toSq, DOUBLE_PAWN_PUSH, b.whiteMove);
        doublePush &= (doublePush - 1);
    }
    while(captureRight){
        int toSq = std::countr_zero(captureRight);
        int fromSq = toSq + ((b.whiteMove) ? -9 : 9);
        int flag = (toSq == b.enPassantSq) ? EPCAPTURE : CAPTURE;
        addPawnMove(moveList, fromSq, toSq, flag, b.whiteMove);
        captureRight &= (captureRight - 1);
    }
    while(captureLeft){
        int toSq = std::countr_zero(captureLeft);
        int fromSq = toSq + ((b.whiteMove) ? -7 : 7);
        int flag = (toSq == b.enPassantSq) ? EPCAPTURE : CAPTURE;
        addPawnMove(moveList, fromSq, toSq, flag, b.whiteMove);
        captureLeft &= (captureLeft - 1);
    }
}