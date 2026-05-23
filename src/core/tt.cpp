#include "tt.h"

TTEntry TT[TT_SIZE];

void initTT(){
    clearTT();
}

void clearTT(){
    for(int i = 0; i < TT_SIZE; i++){
        TT[i].hashKey = 0ULL;
        TT[i].depth = -1;
        TT[i].score = 0;
        TT[i].flag = 0;
        TT[i].bestMove = createMove(0, 0, 0);
    }
}

void storeTT(Bitboard hashKey, int depth, int score, int flag, Move bestMove){
    int index = hashKey % TT_SIZE;
    TT[index].hashKey = hashKey;
    TT[index].depth = depth;
    TT[index].score = score;
    TT[index].flag = flag;
    TT[index].bestMove = bestMove;
}

bool probeTT(Bitboard hashKey, int depth, int alpha, int beta, int& returnScore, Move& bestMove){
    int index = hashKey % TT_SIZE;
    if(TT[index].hashKey == hashKey){
        bestMove = TT[index].bestMove;
        if(TT[index].depth >= depth){
            int score = TT[index].score;

            if(TT[index].flag == TT_EXACT){
                returnScore = score;
                return true;
            }
            if(TT[index].flag == TT_ALPHA && score <= alpha){
                returnScore = alpha;
                return true;
            }
            if(TT[index].flag == TT_BETA && score >= beta){
                returnScore = beta;
                return true;
            }
        }
    }
    return false; //No valid entry found
}