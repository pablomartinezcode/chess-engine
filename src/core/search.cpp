#include "search.h"
#include "movegen.h"
#include "evaluate.h"
#include <iostream>
#include <algorithm> //std::sort
#include <chrono>
#include "tt.h"

namespace {
    bool g_timeLimitedSearch = false;
    std::chrono::steady_clock::time_point g_stopTime;
    bool g_searchAborted = false;
}

bool searchTimeUp() {
    if (!g_timeLimitedSearch) return false;
    return std::chrono::steady_clock::now() >= g_stopTime;
}

bool isCapture(Move m) {
    int flags = m.getFlags();
    return (flags == CAPTURE || flags == EPCAPTURE || (flags >= PROMO_CAPTURE_N && flags <= PROMO_CAPTURE_Q));
}

int getPieceAt(const Board& b, int sq){
    for(int i = 0; i < 12; i++){
        if((b.pieces[i] >> sq) & 1ULL){
            return i;
        }
    }
    return -1; //No piece found
}

int scoreMove(const Board& b, Move m, Move ttMove){
    if (m.data == ttMove.data && m.data != 0) {
        return 10000; //Highest score for transposition table move
    }
    
    if(isCapture(m)){
        int victim = getPieceAt(b, m.getTo());
        int attacker = getPieceAt(b, m.getFrom());

        int victimValue = (victim != -1) ? getPieceValue(victim) : PAWN_VALUE; //Assume en passant captures are worth a pawn
        int attackerValue = (attacker != -1) ? getPieceValue(attacker) : PAWN_VALUE;

        return 1000 + victimValue - attackerValue; //Captures are scored based on the value of the victim and attacker
    }
    return 0; //Non-captures are scored as 0 for now
}

void orderMoves(const Board& b, std::vector<Move>& moves, Move ttMove = createMove(0, 0, 0)){
    std::sort(moves.begin(), moves.end(), [&b, ttMove](Move a, Move b_move){
        return scoreMove(b, a, ttMove) > scoreMove(b, b_move, ttMove);
    });
}

int quiescence(Board& b, int alpha, int beta){
    if(searchTimeUp()){
        g_searchAborted = true;
        return alpha;
    }

    int eval = evaluate(b);
    int stand_pat = (b.whiteMove) ? eval : -eval;
    if(stand_pat >= beta){
        return beta;
    }
    if(stand_pat > alpha){
        alpha = stand_pat;
    }

    std::vector<Move> moves = generateLegalMoves(b);
    orderMoves(b, moves); //Order moves to improve alpha-beta pruning
    
    for(Move m : moves){
        if(!isCapture(m)) continue; //Only consider captures in quiescence search

        UndoInfo undo = makeMove(b, m);
        int score = -quiescence(b, -beta, -alpha);
        unMakeMove(b, m, undo);

        if(score >= beta){
            return beta;
        }
        if(score > alpha){
            alpha = score;
        }
    }
    return alpha;
}



int negamax(Board& b, int depth, int alpha, int beta){
    if(searchTimeUp()){
        g_searchAborted = true;
        return alpha;
    }

    //Probe transposition table for existing evaluation
    Move ttMove = createMove(0, 0, 0);
    int ttScore = 0;
    if(probeTT(b.hashKey, depth, alpha, beta, ttScore, ttMove)){
        return ttScore;
    }
    
    if(depth == 0) {
        return quiescence(b, alpha, beta);
    }

    std::vector<Move> moves = generateLegalMoves(b);

    if(moves.empty()){
        //Checkmate or stalemate
        int kingSq = std::countr_zero(b.pieces[(b.whiteMove ? KING_W : KING_B)]);
        int opponentSide = (b.whiteMove) ? BLACK : WHITE;

        if(isSquareAttacked(b, kingSq, opponentSide)){
            //Checkmate
            return -MATE_SCORE - depth;
        }
        //Stalemate
        return 0;
    }

    orderMoves(b, moves, ttMove); //Order moves to improve alpha-beta pruning

    int flag = TT_ALPHA;
    Move bestMoveThisPosition = createMove(0, 0, 0);

    for (Move m : moves){
        UndoInfo undo = makeMove(b, m);

        //We pass -beta and -alpha and negate the result because we are switching sides
        int score = -negamax(b, depth - 1, -beta, -alpha);
        unMakeMove(b, m, undo);

        if(score >= beta){
            storeTT(b.hashKey, depth, beta, TT_BETA, m); //Store in transposition table as a beta cutoff
            return beta; //Fail-hard beta cutoff
        }
        if(score > alpha){
            flag = TT_EXACT;
            alpha = score; //Best score so far
            bestMoveThisPosition = m;
        }
    }
    storeTT(b.hashKey, depth, alpha, flag, bestMoveThisPosition);
    return alpha;
}

Move searchBestMove(Board& b, int depth){
    std::vector<Move> moves = generateLegalMoves(b);
    orderMoves(b, moves); //Order moves to improve alpha-beta pruning
    Move bestMove = createMove(0, 0, 0);

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

Move searchBestMoveTimed(Board& b, std::chrono::milliseconds moveTime, int maxDepth){
    if(moveTime.count() <= 0){
        moveTime = std::chrono::milliseconds(1);
    }

    std::vector<Move> rootMoves = generateLegalMoves(b);
    if(rootMoves.empty()) return createMove(0, 0, 0);

    Move bestMove = rootMoves[0];

    g_timeLimitedSearch = true;
    g_stopTime = std::chrono::steady_clock::now() + moveTime;
    g_searchAborted = false;

    for(int depth = 1; depth <= maxDepth; depth++){
        if(searchTimeUp()) break;

        Move iterationBest = createMove(0, 0, 0);
        int alpha = -INF;
        int beta = INF;
        int bestScore = -INF;

        std::vector<Move> moves = generateLegalMoves(b);
        orderMoves(b, moves);

        for(Move m : moves){
            if(searchTimeUp()){
                g_searchAborted = true;
                break;
            }

            UndoInfo undo = makeMove(b, m);
            int score = -negamax(b, depth - 1, -beta, -alpha);
            unMakeMove(b, m, undo);

            if(g_searchAborted){
                break;
            }

            if(score > bestScore){
                bestScore = score;
                iterationBest = m;
            }

            if(score > alpha){
                alpha = score;
            }
        }

        if(!g_searchAborted && iterationBest.data != 0){
            bestMove = iterationBest;
        } else {
            break;
        }
    }

    g_timeLimitedSearch = false;
    return bestMove;
}
