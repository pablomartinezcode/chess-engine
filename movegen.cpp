#include "movegen.h"



void addPawnMove(std::vector<Move>& moveList, int from, int to, int flag, bool isWhite) {
    bool isPromotion = (isWhite && to > 55) || (!isWhite && to < 8);

    if (isPromotion) {
        // If the base flag was CAPTURE, we use the PROMO_CAPTURE versions
        int base = (flag == CAPTURE) ? 12 : 8;
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

Bitboard knightMoveTable[64];

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

        knightMoveTable[sq] = moves;
    }

}

void generateKnightMoves(const Board& b, std::vector<Move>& moveList){
    Bitboard knights = (b.whiteMove) ? b.pieces[KNIGHT_W] : b.pieces[KNIGHT_B];
    Bitboard myPieces = (b.whiteMove) ? b.whitePieces : b.blackPieces;

    while(knights){
        int fromSq = popLSB(knights);

        Bitboard moves = knightMoveTable[fromSq] & ~myPieces;
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

void generateQueenMoves(const Board& b, std::vector<Move>& moveList){
    Bitboard queens = (b.whiteMove) ? b.pieces[QUEEN_W] : b.pieces[QUEEN_B];
    Bitboard myPieces = (b.whiteMove) ? b.whitePieces : b.blackPieces;
    while(queens){
        int fromSq = popLSB(queens);
        Bitboard bishopBlockers = b.allPieces & bishopMask[fromSq];
        Bitboard rookBlockers = b.allPieces & rookMask[fromSq];
        int bishopBits = std::popcount(bishopMask[fromSq]);
        int bishopShift = 64 - bishopBits;
        int bishopIndex = (bishopBlockers * BISHOP_MAGICS[fromSq]) >> bishopShift;
        Bitboard moves = bishopMoveTable[fromSq][bishopIndex] & ~myPieces;

        int rookBits = std::popcount(rookMask[fromSq]);
        int rookShift = 64 - rookBits;
        int rookIndex = (rookBlockers * ROOK_MAGICS[fromSq]) >> rookShift;
        moves |= rookMoveTable[fromSq][rookIndex] & ~myPieces;
        while(moves){
            int toSq = popLSB(moves);
            int flag = ((1ULL << toSq) & b.allPieces) ? CAPTURE : QUIET;
            moveList.push_back(createMove(fromSq, toSq, flag));
        }
    }
}

Bitboard kingMoveTable[64];

void initKingMoveTable(){
    for (int sq = 0; sq < 64; sq++){
        Bitboard moves = 0ULL;
        Bitboard king = (1ULL << sq);

        moves |= (king << 8) | (king >> 8);
        if(!(king & FILE_A)) moves |= (king >> 1) | (king << 7) | (king >> 9);
        if(!(king & FILE_H)) moves |= (king << 1) | (king << 9) | (king >> 7);
        
        kingMoveTable[sq] = moves;
    }
}

void generateKingMoves(const Board& b, std::vector<Move>& moveList){
    Bitboard king = (b.whiteMove) ? b.pieces[KING_W] : b.pieces[KING_B];
    Bitboard myPieces = (b.whiteMove) ? b.whitePieces : b.blackPieces;

    int fromSq = popLSB(king);
    Bitboard moves = kingMoveTable[fromSq] & ~myPieces;

    while(moves){
        int toSq = popLSB(moves);
        int flag = ((1ULL << toSq) & b.allPieces) ? CAPTURE : QUIET;
        moveList.push_back(createMove(fromSq, toSq, flag));
    }

    int opponent = b.whiteMove ? BLACK : WHITE;
    if(b.whiteMove){
        //Check if castling rights are in tact, and middle squares are empty
        if ((b.castlingRights & WK) && !(b.allPieces & ((1ULL << f1) | (1ULL << g1)))) {
            //Check if casting squares are under attack
            if (!isSquareAttacked(b, e1, BLACK) && !isSquareAttacked(b, f1, BLACK) && !isSquareAttacked(b, g1, BLACK))
                moveList.push_back(createMove(e1, g1, KING_CASTLE));
        }
        if ((b.castlingRights & WQ) && !(b.allPieces & ((1ULL << d1) | (1ULL << c1) | (1ULL << b1)))) {
            if (!isSquareAttacked(b, e1, BLACK) && !isSquareAttacked(b, d1, BLACK) && !isSquareAttacked(b, c1, BLACK))
                moveList.push_back(createMove(e1, c1, QUEEN_CASTLE));
        }
    }else{
        if ((b.castlingRights & BK) && !(b.allPieces & ((1ULL << f8) | (1ULL << g8)))) {
            //Check if casting squares are under attack
            if (!isSquareAttacked(b, e8, WHITE) && !isSquareAttacked(b, f8, WHITE) && !isSquareAttacked(b, g8, WHITE))
                moveList.push_back(createMove(e8, g8, KING_CASTLE));
        }
        //Check if castling rights are in tact
        if ((b.castlingRights & BQ) && !(b.allPieces & ((1ULL << d8) | (1ULL << c8) | (1ULL << b8)))) {
            if (!isSquareAttacked(b, e8, WHITE) && !isSquareAttacked(b, d8, WHITE) && !isSquareAttacked(b, c8, WHITE))
                moveList.push_back(createMove(e8, c8, QUEEN_CASTLE));
        }
    }
}

bool isSquareAttacked(const Board& b, int sq, int attackerSide) {
    // 1. Check Pawns (Reverse the capture logic)
    Bitboard pawns = (attackerSide == WHITE) ? b.pieces[PAWN_W] : b.pieces[PAWN_B];
    if (attackerSide == WHITE) {
        if (((1ULL << sq) >> 7) & pawns & ~FILE_A) return true;
        if (((1ULL << sq) >> 9) & pawns & ~FILE_H) return true;
    } else {
        if (((1ULL << sq) << 7) & pawns & ~FILE_H) return true;
        if (((1ULL << sq) << 9) & pawns & ~FILE_A) return true;
    }

    // 2. Check Knights
    if (knightMoveTable[sq] & ((attackerSide == WHITE) ? b.pieces[KNIGHT_W] : b.pieces[KNIGHT_B])) return true;

    // 3. Check King
    if (kingMoveTable[sq] & ((attackerSide == WHITE) ? b.pieces[KING_W] : b.pieces[KING_B])) return true;

    // 4. Check Bishops/Queens (Sliders)
    Bitboard bishopAttacks = bishopMoveTable[sq][(b.allPieces & bishopMask[sq]) * BISHOP_MAGICS[sq] >> (64 - std::popcount(bishopMask[sq]))];
    Bitboard enemyBishops = (attackerSide == WHITE) ? (b.pieces[BISHOP_W] | b.pieces[QUEEN_W]) : (b.pieces[BISHOP_B] | b.pieces[QUEEN_B]);
    if (bishopAttacks & enemyBishops) return true;

    // 5. Check Rooks/Queens (Sliders)
    Bitboard rookAttacksBB = rookMoveTable[sq][(b.allPieces & rookMask[sq]) * ROOK_MAGICS[sq] >> (64 - std::popcount(rookMask[sq]))];
    Bitboard enemyRooks = (attackerSide == WHITE) ? (b.pieces[ROOK_W] | b.pieces[QUEEN_W]) : (b.pieces[ROOK_B] | b.pieces[QUEEN_B]);
    if (rookAttacksBB & enemyRooks) return true;

    return false;
}

std::vector<Move> generateLegalMoves(Board &b){
    std::vector<Move> pseudoMoves;
    std::vector<Move> legalMoves;

    generatePawnMoves(b, pseudoMoves);
    generateKnightMoves(b, pseudoMoves);
    generateBishopMoves(b, pseudoMoves);
    generateRookMoves(b, pseudoMoves);
    generateQueenMoves(b, pseudoMoves);
    generateKingMoves(b, pseudoMoves);

    for(Move m : pseudoMoves){
        Board tempBoard = b;
        makeMove(tempBoard, m);

        int opponentSide = b.whiteMove ? BLACK : WHITE;
        Bitboard kingBB = tempBoard.pieces[(b.whiteMove) ? KING_W : KING_B];
        int kingSq = std::countr_zero(kingBB);

        if(!isSquareAttacked(tempBoard, kingSq, opponentSide)){
            legalMoves.push_back(m);
        }
    }

    return legalMoves;
}