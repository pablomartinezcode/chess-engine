#include "magic.h"
#include "board.h"

Bitboard bishopMask[64];
Bitboard bishopMagic[64];
Bitboard bishopAttackTable[64][512];

//Creates bishopMask for each possible square 0-63
// Example bishop mask for d4
// 8  . . . . . . . .
// 7  . . . . . . x .
// 6  . x . . . x . .
// 5  . . x . x . . .
// 4  . . . . . . . .
// 3  . . x . x . . .
// 2  . x . . . x . .
// 1  . . . . . . . .
//    a b c d e f g h
void initBishopMask(){
    for(int sq = 0; sq < 64; sq++){
        Bitboard b = 0ULL;
        Bitboard bishop = (1ULL << sq);
        for(int i = 1; i <= 7; i++){
            if(!(bishop & FILE_H || b & FILE_H)) b |= (bishop << (9*i)) | (bishop >> (7*i));
            if(!(bishop & FILE_A || b & FILE_A)) b |= (bishop >> (9*i)) | (bishop << (7*i));
        }
        bishopMask[sq] = b & ~PERIMETER;
    }
}

int popLSB(Bitboard& b){
    int sq = std::countr_zero(b);
    b &= (b - 1);
    return sq;
}


//Helper function that takes in creates a possible mapping of blockers based on the binary of index
Bitboard setBlockers(int index, int bits, Bitboard mask){
    Bitboard blockers = 0ULL;
    Bitboard tempMask = mask;

    for(int i = 0; i < bits; i++){
        int sq = popLSB(tempMask);

        if(index & (1U << i)) blockers |= (1ULL << sq);
    }
    return blockers;
}

Bitboard bishopAttacks(int sq, Bitboard blockers){
    Bitboard attacks = 0ULL;

    int rank = sq/8, file = sq%8;

    //North-East
    for(int r = rank + 1, f = file + 1; r <= 7 && f <= 7; r++, f++ ){
        int targetSq = r*8 + f;
        attacks |= (1ULL << targetSq);

        if(blockers & (1ULL << targetSq)) break;
    }

    
    //North-West 
    for(int r = rank + 1, f = file - 1; r <= 7 && f >= 0; r++, f-- ){
        int targetSq = r*8 + f;
        attacks |= (1ULL << targetSq);

        if(blockers & (1ULL << targetSq)) break;
    }

    //South-West 
    for(int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f-- ){
        int targetSq = r*8 + f;
        attacks |= (1ULL << targetSq);

        if(blockers & (1ULL << targetSq)) break;
    }

    //South-East 
    for(int r = rank - 1, f = file + 1; r >= 0 && f <= 7; r--, f++ ){
        int targetSq = r*8 + f;
        attacks |= (1ULL << targetSq);

        if(blockers & (1ULL << targetSq)) break;
    }

    return attacks;
}

//Takes in a square and two arrays. Saves all possible combinations of blockers and respective attacks to arrays;
int buildBishopAttackData(int sq, Bitboard blockers[512], Bitboard attacks[512]){
    Bitboard mask = bishopMask[sq];
    int relevantBits = std::popcount(mask);
    int possibleCombinations = (1U << relevantBits); //Maxes out a 2^9 possible blocker combinations

    for(int i = 0; i < possibleCombinations; i++){
        blockers[i] = setBlockers(i, relevantBits, mask);
        attacks[i] = bishopAttacks(sq, blockers[i]);
    }

    return possibleCombinations;
}

bool testBishopMagicNumber(int sq, Bitboard magic, const Bitboard blockers[512], const Bitboard attacks[512], int possibleCombinations){
    Bitboard usedAttacks[512] = {};
    bool slotUsed[512] = {};

    int relevantBits = std::popcount(bishopMask[sq]);
    int shift = 64 - relevantBits;

    for(int i = 0; i < possibleCombinations; i++){
        int index = (blockers[i] * magic) >> shift;

        if(!slotUsed[index]){
            slotUsed[index] = true;
            usedAttacks[index] = attacks[i];
        }else if(usedAttacks[index] != attacks[i]){
            return false;
        }
    }

    return true;
}

Bitboard randomU64(){
    static std::mt19937_64 rng(0xC0FFEE123456789ULL);//0xC0FFEE123456789ULL
    return rng();
}

Bitboard randomU64FewBits(){
    return randomU64() & randomU64() & randomU64();
}

Bitboard findBishopMagicForSquare(int sq){
    Bitboard blockers[512];
    Bitboard attacks[512];

    int possibleCombinations = buildBishopAttackData(sq, blockers, attacks);
    Bitboard mask = bishopMask[sq];

    for(int attempt = 0; attempt < 100000000; attempt++){
        if(attempt % 100000 == 0){
            std::cout << "Attempt " << attempt << '\n';
        }
        Bitboard candidate = randomU64FewBits();

        if(std::popcount((mask * candidate) & 0xFF00000000000000ULL) < 6){
            continue;
        }

        if(testBishopMagicNumber(sq, candidate, blockers, attacks, possibleCombinations)){
            return candidate;
        }
    }
    return 0ULL;
}

void findAllMagicSquares(){
    for(int sq = 0; sq < 64; sq++){
		Bitboard magic = findBishopMagicForSquare(sq);
		std::cout << "Magic for square "<< sq << " :0x" << std::hex << magic << std::dec << '\n';
	}
}
