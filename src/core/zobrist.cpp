#include "zobrist.h"
#include "magic.h"

Bitboard pieceKeys[12][64];
Bitboard enPassantKeys[64];
Bitboard castlingKeys[16];
Bitboard sideKey;

void initZobrist(){
    for(int p = 0; p < 12; p++){
        for(int sq = 0; sq < 64; sq++){
            pieceKeys[p][sq] = randomU64();
        }
    }
    for(int sq = 0; sq < 64; sq++){
        enPassantKeys[sq] = randomU64();
    }
    for(int i = 0; i < 16; i++){
        castlingKeys[i] = randomU64();
    }
    sideKey = randomU64();
}

Bitboard generateHash(const Board &b){
    Bitboard finalKey = 0ULL;

    for(int p = 0; p < 12; p++){
        Bitboard pieces = b.pieces[p];
        while(pieces){
            int sq = popLSB(pieces);
            finalKey ^= pieceKeys[p][sq];
        }
    }

    if(b.enPassantSq != -1){
        finalKey ^= enPassantKeys[b.enPassantSq];
    }

    finalKey ^= castlingKeys[b.castlingRights];

    if(b.whiteMove){
        finalKey ^= sideKey;
    }

    return finalKey;
}