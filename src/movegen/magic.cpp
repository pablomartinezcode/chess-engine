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
        // if(attempt % 100000 == 0){
        //     std::cout << "Attempt " << attempt << '\n';
        // }
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

void findAllBishopMagicSquares(){
    for(int sq = 0; sq < 64; sq++){
		Bitboard magic = findBishopMagicForSquare(sq);
		std::cout << "0x" << std::hex << magic << std::dec << ", ";
	}
    std::cout << std::endl;
}


Bitboard rookMask[64];
const Bitboard ROOK_MAGICS[64] = {
    0x4180042214804000ULL, 0x1080102008804000ULL, 0x2200200880420010ULL, 0x4080048108021000ULL, 0x200080201100420ULL, 0x2080240001801200ULL, 0x1400100401080082ULL, 0x200040081082042ULL,
    0x1800080400025ULL, 0x2122402000401002ULL, 0x4000808010002000ULL, 0x1004801000800802ULL, 0x2800800804402ULL, 0x2003124182200ULL, 0x204000824019002ULL, 0x14800045000880ULL,
    0x40a0008080004000ULL, 0x4220008040008020ULL, 0x384a0010208200ULL, 0x804200200a0011ULL, 0x1001110008010004ULL, 0x8040808004000200ULL, 0x2210100020004ULL, 0x220200140140a5ULL,
    0x4908400080009429ULL, 0x200840100040ULL, 0x108220200134480ULL, 0xa01610010009a102ULL, 0x2040080080280ULL, 0x301400801201004ULL, 0x4000020080800100ULL, 0xa08004200240081ULL,
    0x40008c800020ULL, 0x4008402005401000ULL, 0x2004200088801000ULL, 0x800100a02004020ULL, 0x40080800800ULL, 0x4004824012010ULL, 0x20001041040008a2ULL, 0x852008062000401ULL,
    0x410400882218001ULL, 0x2240200050004000ULL, 0x204200820010ULL, 0x81011000090020ULL, 0x1000800850010ULL, 0x204000200808004ULL, 0x2000020001008080ULL, 0x8c00011840820004ULL,
    0x40004611a1008200ULL, 0x4000204081020200ULL, 0xa80801004200880ULL, 0xd0040041080240ULL, 0x8020040040040ULL, 0x486001008040200ULL, 0x10418218100400ULL, 0x40010c00608200ULL,
    0x2001029140248003ULL, 0x210400010210089ULL, 0x401082200a02ULL, 0x604005009000a009ULL, 0x4c02000820100402ULL, 0x422004421081082ULL, 0x8582008108021054ULL, 0x440844c240902ULL
};

void initRookMask(){
    for (int sq = 0; sq < 64; sq++) {
        Bitboard mask = 0ULL;
        int rank = sq / 8;
        int file = sq % 8;

        // North
        for (int r = rank + 1; r <= 6; r++) {
            mask |= (1ULL << (r * 8 + file));
        }

        // South
        for (int r = rank - 1; r >= 1; r--) {
            mask |= (1ULL << (r * 8 + file));
        }

        // East
        for (int f = file + 1; f <= 6; f++) {
            mask |= (1ULL << (rank * 8 + f));
        }

        // West
        for (int f = file - 1; f >= 1; f--) {
            mask |= (1ULL << (rank * 8 + f));
        }

        rookMask[sq] = mask;
    }
}

Bitboard rookAttacks(int sq, Bitboard blockers){
    Bitboard attacks = 0ULL;

    int rank = sq/8, file = sq%8;

    //North
    for(int r  = rank + 1; r <= 7; r++){
        int targetSq = r*8 + file;
        attacks |= (1ULL << targetSq);

        if(blockers & (1ULL << targetSq)) break;
    }

    //South
    for(int r  = rank - 1; r >= 0; r--){
        int targetSq = r*8 + file;
        attacks |= (1ULL << targetSq);

        if(blockers & (1ULL << targetSq)) break;
    }

    //East
    for(int f = file + 1; f <= 7; f++){
        int targetSq = rank*8 + f;
        attacks |= (1ULL << targetSq);

        if(blockers & (1ULL << targetSq)) break;
    }

    //West
    for(int f = file - 1; f >= 0; f--){
        int targetSq = rank*8 + f;
        attacks |= (1ULL << targetSq);

        if(blockers & (1ULL << targetSq)) break;
    }

    return attacks;
}

int buildRookAttackData(int sq, Bitboard blockers[4096], Bitboard attacks[4096]){
    Bitboard mask = rookMask[sq];
    int relevantBits = std::popcount(mask);
    int possibleCombinations = (1U << relevantBits);

    for(int i = 0; i < possibleCombinations; i++){
        blockers[i] = setBlockers(i, relevantBits, mask);
        attacks[i] = rookAttacks(sq, blockers[i]);
    }

    return possibleCombinations;
}

bool testRookMagicNumber(int sq, Bitboard magic, const Bitboard blockers[4096], const Bitboard attacks[4096], int possibleCombinations){
    Bitboard usedAttacks[4096] = {};
    bool slotUsed[4096] = {};

    int relevantBits = std::popcount(rookMask[sq]);
    int shift = 64 - relevantBits;

    for(int i = 0; i < possibleCombinations; i++){
        int index  = (blockers[i] * magic) >> shift;

        if(!slotUsed[index]){
            slotUsed[index] = true;
            usedAttacks[index] = attacks[i];
        }else if(usedAttacks[index] != attacks[i]){
            return false;
        }
    }
    return true;

}

Bitboard findRookMagicForSquare(int sq){
    Bitboard blockers[4096];
    Bitboard attacks[4096];

    int possibleCombinations = buildRookAttackData(sq, blockers, attacks);
    Bitboard mask = rookMask[sq];

    for(int attempt = 0; attempt < 100000000; attempt++){
        // if(attempt % 100000 == 0){
        //     std::cout << "Attempt " << attempt << '\n';
        // }

        Bitboard candidate = randomU64FewBits();
        if(std::popcount((mask * candidate) & 0xFFFF000000000000ULL) < 12){
            continue;
        }

        if(testRookMagicNumber(sq, candidate, blockers, attacks, possibleCombinations)){
            return candidate;
        }
    }
    return 0ULL;
}

void findAllRookMagicSquares(){
    for(int sq = 0; sq < 64; sq++){
        Bitboard magic = findRookMagicForSquare(sq);
        std::cout << "0x" << std::hex << magic << std::dec << "ULL, ";
    }
    std::cout << std::endl;
}
