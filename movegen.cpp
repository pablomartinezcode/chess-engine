#include "movegen.h"
#include <bit>

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
        captureRight = (pawns << 9) & b.blackPieces & ~FILE_A;
        captureLeft = (pawns << 7) & b.blackPieces & ~FILE_H;
        //IMPLEMENT WHILE LOOP FOR CAPTUR ERIGHT AND LEFT .
        
    }else{
        pawns = b.pieces[PAWN_B];
        singlePush = (pawns >> 8) & ~b.allPieces;
        doublePush = (singlePush >> 8) & ~b.allPieces & 0x000000FF00000000ULL;
        captureRight = (pawns >> 9) & b.whitePieces & ~FILE_H;
        captureLeft = (pawns >> 7) & b.whitePieces & ~FILE_A;
    }

    while(singlePush){
        //Find index of next 1 bit
        int toSq = std::countr_zero(singlePush);
        int fromSq  = toSq + ((b.whiteMove) ? -8 : 8);
        
        //      White Promotion                 Black Promotion
        if((toSq > 55 && b.whiteMove) || (toSq < 8 && !b.whiteMove)){
            moveList.push_back(createMove(fromSq, toSq, PROMOTION_B));
            moveList.push_back(createMove(fromSq, toSq, PROMOTION_R));
            moveList.push_back(createMove(fromSq, toSq, PROMOTION_N));
            moveList.push_back(createMove(fromSq, toSq, PROMOTION_Q));
        }else{
            moveList.push_back(createMove(fromSq, toSq, QUIET));
        }
        singlePush &= (singlePush - 1);
    }
    while(doublePush){
        int toSq = std::countr_zero(doublePush);
        int fromSq = toSq + ((b.whiteMove) ? -16 : 16);
        moveList.push_back(createMove(fromSq, toSq, DOUBLE_PAWN_PUSH));
        doublePush &= (doublePush - 1);
    }
    while(captureRight){
        int toSq = std::countr_zero(captureRight);
        int fromSq = toSq + ((b.whiteMove) ? -9 : 9);
        if((toSq > 55 && b.whiteMove) || (toSq < 8 && !b.whiteMove)){
            moveList.push_back(createMove(fromSq, toSq, PROMO_CAPTURE_B));
            moveList.push_back(createMove(fromSq, toSq, PROMO_CAPTURE_N));
            moveList.push_back(createMove(fromSq, toSq, PROMO_CAPTURE_Q));
            moveList.push_back(createMove(fromSq, toSq, PROMO_CAPTURE_R));
        }else{
            moveList.push_back(createMove(fromSq, toSq, CAPTURE));
        }
        captureRight &= (captureLeft - 1);
    }
    while(captureLeft){
        int toSq = std::countr_zero(captureLeft);
        int fromSq = toSq + ((b.whiteMove) ? -7 : 7);

        if((toSq > 55 && b.whiteMove) || (toSq < 8 && !b.whiteMove)){
            moveList.push_back(createMove(fromSq, toSq, PROMO_CAPTURE_B));
            moveList.push_back(createMove(fromSq, toSq, PROMO_CAPTURE_N));
            moveList.push_back(createMove(fromSq, toSq, PROMO_CAPTURE_Q));
            moveList.push_back(createMove(fromSq, toSq, PROMO_CAPTURE_R));
        }else{
            moveList.push_back(createMove(fromSq, toSq, CAPTURE));
        }
        captureLeft &= (captureLeft - 1);
    }
    
}