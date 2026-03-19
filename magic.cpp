#include "magic.h"
#include "board.h"

Bitboard bishopMask[64];
const Bitboard BISHOP_MAGICS[64]{
    0x1410200e00504108ULL, 0x1004c81084008002ULL, 0x1200a401028002ULL, 0x8208022008024ULL, 0x8040420580000c2ULL, 0x21104202a1000ULL, 0x84882012a000ULL, 0xa800808050108400ULL,
    0x8001112001440080ULL, 0x209590104009204ULL, 0x5124880234002e32ULL, 0x2000082080200200ULL, 0x8040420201840ULL, 0x210148400268ULL, 0x10822508401ULL, 0x2010210048444400ULL,
    0x210004104481880ULL, 0x808802001440080ULL, 0x5124000848002300ULL, 0x1015260404008040ULL, 0x158884400a00900ULL, 0x4010000b0121008ULL, 0x12010d0404020208ULL, 0x802220845000ULL,
    0x2120080020a85104ULL, 0x8244600a02080100ULL, 0x18500020408480ULL, 0x280880200a0080ULL, 0x188840000802002ULL, 0x81060211008080ULL, 0x2008260001208220ULL, 0x20280002c0100ULL,
    0x8010191800047002ULL, 0xc012002040441ULL, 0x10001c0208100080ULL, 0x4843100820840400ULL, 0x2240410040040040ULL, 0x4080220020480ULL, 0xd00c00c091840404ULL, 0x882140b01202498ULL,
    0xd010420620c2400ULL, 0x802008460000421ULL, 0x4080120101001010ULL, 0x1400004200804800ULL, 0x200040ab2003400ULL, 0xa00208100500a220ULL, 0x28108a1884001922ULL, 0x10020080225103ULL,
    0x21040104408020ULL, 0x841401048491ULL, 0x88020a4100a11ULL, 0x23001020880000ULL, 0x40182a0001ULL, 0x8202a100d220804ULL, 0xa0418808009088e0ULL, 0x305c80200420009ULL,
    0x205a00804c022100ULL, 0x120024044146004ULL, 0x14514042009000ULL, 0x101504041048800ULL, 0x1000000d50203200ULL, 0x2102084013200ULL, 0x8400401220230ULL, 0x20c4840808082080ULL
};


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
Bitboard setBishopBlockers(int index, int bits, Bitboard mask){
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
        blockers[i] = setBishopBlockers(i, relevantBits, mask);
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
    static std::mt19937_64 rng(0xFFFFFFFFFFFFFFFULL);//0xC0FFEE123456789ULL
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
