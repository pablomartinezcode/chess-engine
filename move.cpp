#include "move.h"
#include "movegen.h"

Move createMove(int from, int to, int flags){
    Move m;
    m.data = from | (to << 6) | (flags << 12);
    return m;
};

int parseSquare(std::string sq){
    int file = sq[0] - 'a';//['a'-'h']
    int rank = sq[1] - '1';//['1'-'8']

    return (rank*8) + file;
}

Move parseMove(std::string moveStr, Board &b){
    std::vector<Move> moveList = generateLegalMoves(b);

    int from  = parseSquare(moveStr.substr(0,2));
    int to  = parseSquare(moveStr.substr(2, 2));

    for(const Move& m : moveList){
        if(m.getFrom() == from && m.getTo() == to){
            if(moveStr.length() == 5){
                int f = m.getFlags();
                char promo = moveStr[4];
                if (promo == 'q' && (f == PROMOTION_Q || f == PROMO_CAPTURE_Q)) return m;
                if (promo == 'r' && (f == PROMOTION_R || f == PROMO_CAPTURE_R)) return m;
                if (promo == 'b' && (f == PROMOTION_B || f == PROMO_CAPTURE_B)) return m;
                if (promo == 'n' && (f == PROMOTION_N || f == PROMO_CAPTURE_N)) return m;
                continue;
            }
            return m;
        }
    }

    return createMove(0, 0, 0);
}
