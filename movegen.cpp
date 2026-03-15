#include "movegen.h"
#include <bit>

void generatePawnMoves(const Board& b, std::vector<Move>& moveList){
    Bitboard pawns;
    Bitboard singlePush;
    Bitboard doublePush;
    if(b.whiteMove){
        pawns = b.pieces[PAWN_W];
        singlePush = (pawns << 8) & ~b.allPieces;
        doublePush = (pawns << 16) & ~b.allPieces;
        while(singlePush){
            //Find index of next 1 bit
            int toSq = std::countr_zero(singlePush);
            int fromSq  = toSq - 8;
            moveList.push_back(createMove(fromSq, toSq, 0));
            singlePush &= (singlePush - 1);
        }
        while(doublePush){
            int toSq = std::countr_zero(doublePush);
            int fromSq = toSq - 16;
            moveList.push_back(createMove(fromSq, toSq, 0));
            doublePush &= (doublePush - 1);
        }
    }else{
        pawns = b.pieces[PAWN_B];
        singlePush = (pawns >> 8) & ~b.allPieces;
        doublePush = (pawns >> 16) & ~b.allPieces;
        while(singlePush){
            //Find index of next 1 bit
            int toSq = std::countr_zero(singlePush);
            int fromSq  = toSq + 8;
            moveList.push_back(createMove(fromSq, toSq, 0));
            singlePush &= (singlePush - 1);
        }
        while(doublePush){
            int toSq = std::countr_zero(doublePush);
            int fromSq = toSq + 16;
            moveList.push_back(createMove(fromSq, toSq, 0));
            doublePush &= (doublePush - 1);
        }
    }
    
}