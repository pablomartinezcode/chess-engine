#include "search.h"
#include "movegen.h"
#include "evaluate.h"
#include <algorithm>
#include <array>
#include <chrono>
#include "tt.h"

namespace {
    constexpr int MAX_PLY = 128;
    constexpr int TT_MOVE_SCORE = 1'000'000;
    constexpr int WINNING_CAPTURE_BASE = 500'000;
    constexpr int CAPTURE_BASE = 300'000;
    constexpr int KILLER_1_SCORE = 200'000;
    constexpr int KILLER_2_SCORE = 190'000;

    bool g_timeLimitedSearch = false;
    std::chrono::steady_clock::time_point g_stopTime;
    bool g_searchAborted = false;

    std::array<std::array<Move, 2>, MAX_PLY> g_killers{};
    std::array<std::array<std::array<int, 64>, 64>, 2> g_history{};

    bool searchTimeUp() {
        return g_timeLimitedSearch && (std::chrono::steady_clock::now() >= g_stopTime);
    }

    void resetHeuristics() {
        for (auto& plyKillers : g_killers) {
            plyKillers[0] = createMove(0, 0, 0);
            plyKillers[1] = createMove(0, 0, 0);
        }
        for (auto& sideTable : g_history) {
            for (auto& fromTable : sideTable) {
                fromTable.fill(0);
            }
        }
    }
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
    return -1;
}

int scoreMove(const Board& b, Move m, Move ttMove, int ply){
    if (m.data == ttMove.data && m.data != 0) {
        return TT_MOVE_SCORE;
    }

    if (isCapture(m)) {
        int victim = getPieceAt(b, m.getTo());
        int attacker = getPieceAt(b, m.getFrom());
        int victimValue = (victim != -1) ? getPieceValue(victim) : PAWN_VALUE;
        int attackerValue = (attacker != -1) ? getPieceValue(attacker) : PAWN_VALUE;
        int mvvLva = victimValue - attackerValue;
        if (mvvLva >= 0) return WINNING_CAPTURE_BASE + mvvLva;
        return CAPTURE_BASE + mvvLva;
    }

    if (ply < MAX_PLY) {
        if (m.data == g_killers[ply][0].data && m.data != 0) return KILLER_1_SCORE;
        if (m.data == g_killers[ply][1].data && m.data != 0) return KILLER_2_SCORE;
    }

    int side = b.whiteMove ? WHITE : BLACK;
    return g_history[side][m.getFrom()][m.getTo()];
}

void orderMoves(const Board& b, std::vector<Move>& moves, Move ttMove = createMove(0, 0, 0), int ply = 0){
    std::sort(moves.begin(), moves.end(), [&b, ttMove, ply](Move a, Move b_move){
        return scoreMove(b, a, ttMove, ply) > scoreMove(b, b_move, ttMove, ply);
    });
}

void addKiller(Move m, int ply) {
    if (ply >= MAX_PLY || isCapture(m) || m.data == 0) return;
    if (g_killers[ply][0].data != m.data) {
        g_killers[ply][1] = g_killers[ply][0];
        g_killers[ply][0] = m;
    }
}

void addHistory(const Board& b, Move m, int depth) {
    if (isCapture(m) || m.data == 0) return;
    int side = b.whiteMove ? WHITE : BLACK;
    int& entry = g_history[side][m.getFrom()][m.getTo()];
    entry += depth * depth;
    if (entry > 1'000'000) entry /= 2;
}

int quiescence(Board& b, int alpha, int beta){
    if(searchTimeUp()){
        g_searchAborted = true;
        return alpha;
    }

    int eval = evaluate(b);
    int stand_pat = (b.whiteMove) ? eval : -eval;
    if(stand_pat >= beta) return beta;
    if(stand_pat > alpha) alpha = stand_pat;

    std::vector<Move> moves = generateLegalMoves(b);
    orderMoves(b, moves);

    for(Move m : moves){
        if(!isCapture(m)) continue;

        UndoInfo undo = makeMove(b, m);
        int score = -quiescence(b, -beta, -alpha);
        unMakeMove(b, m, undo);

        if(score >= beta) return beta;
        if(score > alpha) alpha = score;
    }
    return alpha;
}

int negamax(Board& b, int depth, int alpha, int beta, int ply){
    if(searchTimeUp()){
        g_searchAborted = true;
        return alpha;
    }

    Move ttMove = createMove(0, 0, 0);
    int ttScore = 0;
    if(probeTT(b.hashKey, depth, alpha, beta, ttScore, ttMove)) return ttScore;

    if(depth == 0) return quiescence(b, alpha, beta);

    std::vector<Move> moves = generateLegalMoves(b);
    if(moves.empty()){
        int kingSq = std::countr_zero(b.pieces[(b.whiteMove ? KING_W : KING_B)]);
        int opponentSide = (b.whiteMove) ? BLACK : WHITE;
        if(isSquareAttacked(b, kingSq, opponentSide)) return -MATE_SCORE - depth;
        return 0;
    }

    orderMoves(b, moves, ttMove, ply);

    int flag = TT_ALPHA;
    Move bestMoveThisPosition = createMove(0, 0, 0);

    for (Move m : moves){
        UndoInfo undo = makeMove(b, m);
        int score = -negamax(b, depth - 1, -beta, -alpha, ply + 1);
        unMakeMove(b, m, undo);

        if (g_searchAborted) return alpha;

        if(score >= beta){
            addKiller(m, ply);
            addHistory(b, m, depth);
            storeTT(b.hashKey, depth, beta, TT_BETA, m);
            return beta;
        }
        if(score > alpha){
            flag = TT_EXACT;
            alpha = score;
            bestMoveThisPosition = m;
        }
    }

    if(bestMoveThisPosition.data != 0) addHistory(b, bestMoveThisPosition, depth);
    storeTT(b.hashKey, depth, alpha, flag, bestMoveThisPosition);
    return alpha;
}

Move searchBestMove(Board& b, int depth){
    resetHeuristics();

    std::vector<Move> rootMoves = generateLegalMoves(b);
    if (rootMoves.empty()) return createMove(0, 0, 0);

    Move bestMove = rootMoves[0];
    Move pvMove = bestMove;

    for (int currentDepth = 1; currentDepth <= depth; currentDepth++) {
        std::vector<Move> moves = generateLegalMoves(b);
        orderMoves(b, moves, pvMove, 0);

        int alpha = -INF;
        int beta = INF;
        int bestScore = -INF;
        Move iterationBest = createMove(0, 0, 0);

        for (Move m : moves) {
            UndoInfo undo = makeMove(b, m);
            int score = -negamax(b, currentDepth - 1, -beta, -alpha, 1);
            unMakeMove(b, m, undo);

            if(score > bestScore){
                bestScore = score;
                iterationBest = m;
            }
            if(score > alpha) alpha = score;
        }

        if (iterationBest.data != 0) {
            bestMove = iterationBest;
            pvMove = iterationBest;
        }
    }

    return bestMove;
}

Move searchBestMoveTimed(Board& b, std::chrono::milliseconds moveTime, int maxDepth){
    if(moveTime.count() <= 0) moveTime = std::chrono::milliseconds(1);

    std::vector<Move> rootMoves = generateLegalMoves(b);
    if(rootMoves.empty()) return createMove(0, 0, 0);

    resetHeuristics();

    Move bestMove = rootMoves[0];
    Move pvMove = bestMove;

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
        orderMoves(b, moves, pvMove, 0);

        for(Move m : moves){
            if(searchTimeUp()){
                g_searchAborted = true;
                break;
            }

            UndoInfo undo = makeMove(b, m);
            int score = -negamax(b, depth - 1, -beta, -alpha, 1);
            unMakeMove(b, m, undo);

            if(g_searchAborted) break;

            if(score > bestScore){
                bestScore = score;
                iterationBest = m;
            }

            if(score > alpha) alpha = score;
        }

        if(!g_searchAborted && iterationBest.data != 0){
            bestMove = iterationBest;
            pvMove = iterationBest;
        } else {
            break;
        }
    }

    g_timeLimitedSearch = false;
    return bestMove;
}
