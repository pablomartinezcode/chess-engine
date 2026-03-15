#include "move.h"

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

Move parseMove(std::string moveStr){
    int from  = parseSquare(moveStr.substr(0,2));
    int to  = parseSquare(moveStr.substr(2, 2));

    return createMove(from, to, 0);
}
