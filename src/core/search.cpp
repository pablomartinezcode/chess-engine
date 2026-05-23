#include "search.h"
#include "movegen.h"
#include "evaluate.h"
#include <iostream>

int negamax(Board& b, int depth, int alpha, int beta){
    if(depth == 0) {
        //evaluate returns score from white's perspective, so we negate it for black
        int eval = evaluate(b);
        return (b.whiteMove) ? eval : -eval;
    }

    std::vector<Move> moves = generateLegalMoves(b);

    if(moves.empty()){
        //Checkmate or stalemate
        int kingSq = std::countr_zero(b.pieces[(b.whiteMove ? KING_W : KING_B)]);
        int opponentSide = (b.whiteMove) ? BLACK : WHITE;

        if(isSquareAttacked(b, kingSq, opponentSide)){
            //Checkmate
            return -MATE_SCORE + depth;
        }
        //Stalemate
        return 0;
    }

    for (Move m : moves){
        UndoInfo undo = makeMove(b, m);

        //We pass -beta and -alpha and negate the result because we are switching sides
        int score = -negamax(b, depth - 1, -beta, -alpha);
        unMakeMove(b, m, undo);

        if(score >= beta){
            return beta; //Fail-hard beta cutoff
        }
        if(score > alpha){
            alpha = score; //Best score so far
        }
    }
    return alpha;
}

Move searchBestMove(Board& b, int depth){
    std::vector<Move> moves = generateLegalMoves(b);
    Move bestMove;

    int alpha = -INF;
    int beta = INF;
    int bestScore = -INF;

    for (Move m : moves){
        UndoInfo undo = makeMove(b, m);
        int score = -negamax(b, depth - 1, -beta, -alpha);
        unMakeMove(b, m, undo);

        if(score > bestScore){
            bestScore = score;
            bestMove = m;
        }
        if(score > alpha){
            alpha = score;
        }
    }
    return bestMove;
}