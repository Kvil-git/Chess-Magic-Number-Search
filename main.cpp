#include <iostream>
#include <stdint.h>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <unordered_map>
#include <limits>
#include <chrono>
#include <string>

using namespace std;

using bitboard = uint64_t;
using magicNumber = uint64_t;

template<class IntegerType> inline IntegerType BIT(const IntegerType &x)
{ return x < sizeof(IntegerType)*8 ? (IntegerType(1) << x) : IntegerType(0); }

template<class IntegerType> inline bool IsBitSet(const IntegerType &objectToCheck, const int &bitIndex)
{ return (objectToCheck >> bitIndex) & IntegerType(1); }

template<class IntegerType> inline void SetBit(IntegerType &objectToSet, const int &bitIndex)
{ objectToSet |= BIT<IntegerType>(bitIndex);  }


bitboard GenerateRookMovesMaskAtSquare(int sqr){
    bitboard output = 0;
    int rookRow = sqr / 8;
    int rookColumn = sqr % 8;
    for(int row = rookRow - 1; row > 0; row--) SetBit(output, row * 8 + rookColumn);
    for(int row = rookRow + 1; row < 7; row++) SetBit(output, row * 8 + rookColumn);
    for(int column = rookColumn - 1; column > 0; column--) SetBit(output, rookRow * 8 + column);
    for(int column = rookColumn + 1; column < 7; column++) SetBit(output, rookRow * 8 + column);
    return output;
}

bitboard GenerateBishopMovesMaskAtSquare(int sqr){
    bitboard output = 0;
    int bishopRow = sqr / 8;
    int bishopColumn = sqr % 8;

    for(int row = bishopRow + 1, column = bishopColumn + 1; row<7 && column<7; row++, column++) SetBit(output, row * 8 + column);
    for(int row = bishopRow + 1, column = bishopColumn - 1; row<7 && column>0; row++, column--) SetBit(output, row * 8 + column);
    for(int row = bishopRow - 1, column = bishopColumn + 1; row>0 && column<7; row--, column++) SetBit(output, row * 8 + column);
    for(int row = bishopRow - 1, column = bishopColumn - 1; row>0 && column>0; row--, column--) SetBit(output, row * 8 + column);
    return output;
}

vector<bitboard> GenerateMoveMasks(bool rookMode){
    vector<bitboard> masks;
    for(int sq=0; sq<64; sq++){
        bitboard output = rookMode ? GenerateRookMovesMaskAtSquare(sq) : GenerateBishopMovesMaskAtSquare(sq);
        masks.push_back(output);
    }
    return masks;
}




uint64_t RandomNumber(){
    uint64_t random =
      (((uint64_t) rand() <<  0) & 0x000000000000FFFFull) ^ 
      (((uint64_t) rand() << 16) & 0x00000000FFFF0000ull) ^ 
      (((uint64_t) rand() << 32) & 0x0000FFFF00000000ull) ^
      (((uint64_t) rand() << 48) & 0xFFFF000000000000ull);
    return random;
}

vector<int> GetSetBitIndices(bitboard num){
    vector<int> indices;
    for(int i=0; i<64; i++)
        if(IsBitSet(num, i)) indices.push_back(i);
    return indices;
}

struct MagicOutput{
    magicNumber number;
    int shift;
    MagicOutput() : number(0), shift(64) {}
};

vector<vector<bitboard>> BlockerIndexArraysArray(64);



void FillBlockerIndexArraysArray(int sqr, bitboard mask){
    vector<int> bits = GetSetBitIndices(mask);
    int numBits = (int)bits.size();
    int subsetCount = 1 << numBits;

    BlockerIndexArraysArray[sqr].clear();
    BlockerIndexArraysArray[sqr].reserve(subsetCount);

    for(int subset = 0; subset < subsetCount; subset++){
        bitboard blockers = 0ull;
        for(int j=0; j<numBits; j++){
            if(subset & (1 << j))
                SetBit(blockers, bits[j]);
        }
        BlockerIndexArraysArray[sqr].push_back(blockers);
    }
}

MagicOutput TryMagicAtSquare(int sqr, bitboard mask, uint64_t candidate){
    vector<bitboard>& blockers = BlockerIndexArraysArray[sqr];
    int relevantBits = (int)GetSetBitIndices(mask).size();

    vector<bitboard> used(1 << relevantBits, 0);
    for(bitboard b : blockers){
        int index = (int)((b * candidate) >> (64 - relevantBits));
        if(used[index] == 0){
            used[index] = b;
        } else if(used[index] != b){
            MagicOutput fail;
            return fail; // shift=64 means invalid
        }
    }
    MagicOutput success;
    success.number = candidate;
    success.shift = 64 - relevantBits;
    return success;
}





void DumpMagics(const vector<MagicOutput>& best, bool rookMode) {
    cout << "\n=== Current Best " << (rookMode ? "Rook" : "Bishop") << " Magics ===" << endl;
    for(int sq=0; sq<64; sq++){
        const MagicOutput& m = best[sq];
        int tableSize = (m.shift < 64) ? (1 << (64 - m.shift)) : -1;
        cout << "Square " << sq 
             << ": Magic=0x" << hex << m.number << dec 
             << " Shift=" << m.shift 
             << " TableSize=" << tableSize << endl;
    }
    cout << "================================\n" << endl;
}


int main(int argc, char** argv) {
    srand((unsigned)time(NULL));

    bool rookMode = true;
    if(argc > 1){
        string arg = argv[1];
        if(arg == "--bishop") rookMode = false;
    }

    vector<bitboard> masks = GenerateMoveMasks(rookMode);

    for(int sq=0; sq<64; sq++)
        FillBlockerIndexArraysArray(sq, masks[sq]);

    vector<MagicOutput> best(64);

    cout << "Searching for " << (rookMode ? "rook" : "bishop") 
         << " magic numbers... (press Ctrl-C to stop)" << endl;

    auto lastDump = chrono::steady_clock::now();

    
    while(true){
        for(int sq=0; sq<64; sq++){
            uint64_t candidate = RandomNumber() & RandomNumber() & RandomNumber(); // 3 number ANDs make results sparser 
            MagicOutput attempt = TryMagicAtSquare(sq, masks[sq], candidate);
            if(attempt.shift < best[sq].shift){
                best[sq] = attempt;
                int tableSize = 1 << (64 - attempt.shift);
                cout << "Square " << sq 
                     << " improved: Magic=0x" << hex << attempt.number << dec 
                     << " Shift=" << attempt.shift 
                     << " TableSize=" << tableSize << endl;
            }
        }

        auto now = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::seconds>(now - lastDump).count();
        if(elapsed >= 5){
            DumpMagics(best, rookMode);
            lastDump = now;
        }
    }

    return 0;
}
