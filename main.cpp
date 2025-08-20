#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstdint>
#include <random>
#include <atomic>
#include <algorithm>
#include <iomanip>

using namespace std;
using namespace chrono;

using bitboard = uint64_t;
using magicNumber = uint64_t;

// Improved random number generation
thread_local mt19937_64 rng(random_device{}());
thread_local uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

// Bit manipulation utilities
template<class IntegerType> inline IntegerType BIT(const IntegerType &x) {
    return (IntegerType(1) << x);
}

template<class IntegerType> inline bool IsBitSet(const IntegerType &objectToCheck, const int &bitIndex) {
    return (objectToCheck >> bitIndex) & 1;
}

template<class IntegerType> inline void SetBit(IntegerType &objectToSet, const int &bitIndex) {
    objectToSet |= BIT<IntegerType>(bitIndex);
}

// Generate rook moves mask (excluding edges)
bitboard GenerateRookMovesMaskAtSquare(int sqr) {
    bitboard output = 0;
    int row = sqr / 8, col = sqr % 8;
    
    // Up (excluding edge)
    for (int r = row + 1; r < 7; r++) 
        SetBit(output, r * 8 + col);
    
    // Down (excluding edge)
    for (int r = row - 1; r > 0; r--) 
        SetBit(output, r * 8 + col);
    
    // Right (excluding edge)
    for (int c = col + 1; c < 7; c++) 
        SetBit(output, row * 8 + c);
    
    // Left (excluding edge)
    for (int c = col - 1; c > 0; c--) 
        SetBit(output, row * 8 + c);
    
    return output;
}

bitboard GenerateBishopMovesMaskAtSquare(int sqr) {
    bitboard output = 0;
    int row = sqr / 8, col = sqr % 8;
    
    // Down-Right (excluding edges)
    for (int r = row + 1, c = col + 1; r < 7 && c < 7; r++, c++) 
        SetBit(output, r * 8 + c);
    
    // Down-Left (excluding edges)
    for (int r = row + 1, c = col - 1; r < 7 && c > 0; r++, c--) 
        SetBit(output, r * 8 + c);
    
    // Up-Right (excluding edges)
    for (int r = row - 1, c = col + 1; r > 0 && c < 7; r--, c++) 
        SetBit(output, r * 8 + c);
    
    // Up-Left (excluding edges)
    for (int r = row - 1, c = col - 1; r > 0 && c > 0; r--, c--) 
        SetBit(output, r * 8 + c);
    
    return output;
}

// Generate actual rook attacks considering blockers
bitboard GenerateRookAttacks(int sq, bitboard blockers) {
    bitboard attacks = 0;
    int row = sq / 8, col = sq % 8;
    
    // Up
    for (int r = row + 1; r < 8; r++) {
        int target = r * 8 + col;
        SetBit(attacks, target);
        if (IsBitSet(blockers, target)) break;
    }
    // Down
    for (int r = row - 1; r >= 0; r--) {
        int target = r * 8 + col;
        SetBit(attacks, target);
        if (IsBitSet(blockers, target)) break;
    }
    // Right
    for (int c = col + 1; c < 8; c++) {
        int target = row * 8 + c;
        SetBit(attacks, target);
        if (IsBitSet(blockers, target)) break;
    }
    // Left
    for (int c = col - 1; c >= 0; c--) {
        int target = row * 8 + c;
        SetBit(attacks, target);
        if (IsBitSet(blockers, target)) break;
    }
    
    return attacks;
}

// Generate actual bishop attacks considering blockers
bitboard GenerateBishopAttacks(int sq, bitboard blockers) {
    bitboard attacks = 0;
    int row = sq / 8, col = sq % 8;
    
    // Down-right
    for (int r = row + 1, c = col + 1; r < 8 && c < 8; r++, c++) {
        int target = r * 8 + c;
        SetBit(attacks, target);
        if (IsBitSet(blockers, target)) break;
    }
    // Down-left
    for (int r = row + 1, c = col - 1; r < 8 && c >= 0; r++, c--) {
        int target = r * 8 + c;
        SetBit(attacks, target);
        if (IsBitSet(blockers, target)) break;
    }
    // Up-right
    for (int r = row - 1, c = col + 1; r >= 0 && c < 8; r--, c++) {
        int target = r * 8 + c;
        SetBit(attacks, target);
        if (IsBitSet(blockers, target)) break;
    }
    // Up-left
    for (int r = row - 1, c = col - 1; r >= 0 && c >= 0; r--, c--) {
        int target = r * 8 + c;
        SetBit(attacks, target);
        if (IsBitSet(blockers, target)) break;
    }
    
    return attacks;
}

// Magic output structure
struct MagicOutput {
    magicNumber number;
    int shift;
    int tableSize;
    MagicOutput() : number(0), shift(64), tableSize(0) {}
};

bitboard BishopMasks[64];
bitboard RookMasks[64];

using BlockerArray = vector<bitboard>;

bool ValidateMagic(int sq, const MagicOutput& magic, bool isBishop, const BlockerArray &blockers) {
    vector<bitboard> table(magic.tableSize, 0);
    for (bitboard b : blockers) {
        size_t index = (b * magic.number) >> magic.shift;
        bitboard attacks = isBishop ? GenerateBishopAttacks(sq, b)
                                    : GenerateRookAttacks(sq, b);
        if (table[index] != 0 && table[index] != attacks) return false;
        table[index] = attacks;
    }
    return true;
}

// Utility functions
vector<int> GetSetBitIndices(bitboard num) {
    vector<int> indices;
    for (int i = 0; i < 64; i++)
        if (IsBitSet(num, i)) indices.push_back(i);
    return indices;
}

uint64_t RandomNumber() {
    return dist(rng);
}

// Generate better magic candidates
uint64_t RandomSparseNumber() {
    // Use more operations for better sparsity
    return RandomNumber() & RandomNumber() & RandomNumber();
}

// Global variables
mutex bestMutex;
vector<MagicOutput> best(64);
atomic<bool> stopThreads(false);
atomic<int> squaresFound(0);

void FillBlockerIndexArray(bitboard mask, BlockerArray &array) {
    vector<int> bits = GetSetBitIndices(mask);
    int n = bits.size();
    int subsetCount = 1 << n;
    array.clear();
    array.reserve(subsetCount);
    
    for (int subset = 0; subset < subsetCount; subset++) {
        bitboard blockers = 0;
        for (int j = 0; j < n; j++)
            if (subset & (1 << j))
                SetBit(blockers, bits[j]);
        array.push_back(blockers);
    }
}

MagicOutput TryMagic(bitboard mask, magicNumber candidate, const BlockerArray &blockers, bool isBishop, int sq) {
    vector<int> bits = GetSetBitIndices(mask);
    int relevantBits = bits.size();
    vector<bitboard> table(1 << relevantBits, 0);

    for (bitboard b : blockers) {
        bitboard attacks = isBishop ? GenerateBishopAttacks(sq, b)
                                    : GenerateRookAttacks(sq, b);
        int index = (int)((b * candidate) >> (64 - relevantBits));
        if (table[index] == 0)
            table[index] = attacks;
        else if (table[index] != attacks)
            return MagicOutput(); // collision
    }

    MagicOutput result;
    result.number = candidate;
    result.shift = 64 - relevantBits;
    result.tableSize = 1 << relevantBits;
    return result;
}

// Worker thread
void Worker(int id, bool bishopMode, const vector<bitboard>& masks, const vector<BlockerArray>& blockers) {
    auto lastDump = steady_clock::now();
    int attempts = 0;
    
    while (!stopThreads) {
        for (int sq = 0; sq < 64 && !stopThreads; sq++) {
            // Skip if already found
            {
                lock_guard<mutex> lock(bestMutex);
                if (best[sq].shift < 64) continue;
            }
            
            attempts++;
            
            // Generate candidate with sparse bits
            uint64_t candidate = RandomSparseNumber();
            
            MagicOutput attempt = TryMagic(masks[sq], candidate, blockers[sq], bishopMode, sq);
            
            if (attempt.shift < 64) {
                if(ValidateMagic(sq, attempt, bishopMode, blockers[sq])){
                    lock_guard<mutex> lock(bestMutex);
                    if (best[sq].shift == 64) {
                        best[sq] = attempt;
                        squaresFound++;
                        
                        // Check if all squares are found
                        if (squaresFound == 64) {
                            stopThreads = true;
                        }
                    }
                }
            }
        }
        
        // Print status every 5 seconds
        auto now = steady_clock::now();
        if (duration_cast<seconds>(now - lastDump).count() >= 5) {
            lock_guard<mutex> lock(bestMutex);
            cout << "\n=== Current Best " << (bishopMode ? "Bishop" : "Rook") << " Magics ===\n";
            cout << "Squares found: " << squaresFound << "/64\n";
            cout << "Thread " << id << " attempts: " << attempts << "\n";
            
            lastDump = now;
        }
    }
}

// Main function
int main(int argc, char** argv) {
    bool bishopMode = false;
    if (argc > 1 && string(argv[1]) == "--bishop") 
        bishopMode = true;

    // Precompute masks and blockers
    vector<bitboard> masks(64);
    vector<BlockerArray> blockers(64);
    
    for (int sq = 0; sq < 64; sq++) {
        masks[sq] = bishopMode ? GenerateBishopMovesMaskAtSquare(sq) 
                               : GenerateRookMovesMaskAtSquare(sq);
        FillBlockerIndexArray(masks[sq], blockers[sq]);
        
        vector<int> bits = GetSetBitIndices(masks[sq]);
        cout << "Square " << sq << " mask has " << bits.size() << " bits\n";
    }

    int threadCount = thread::hardware_concurrency();
    cout << "Running in " << (bishopMode ? "bishop" : "rook") 
         << " mode with " << threadCount << " threads.\n";

    vector<thread> threads;
    for (int i = 0; i < threadCount; i++)
        threads.emplace_back(Worker, i, bishopMode, ref(masks), ref(blockers));

    for (auto &t : threads) 
        t.join();

    // Final output
    cout << "\n=== Final " << (bishopMode ? "Bishop" : "Rook") << " Magics ===\n";
    for (int sq = 0; sq < 64; sq++) {
        auto &m = best[sq];
        cout << "Square " << sq << ": Magic=0x" << hex << setw(16) << setfill('0') << m.number << dec
             << " Shift=" << m.shift
             << " TableSize=" << m.tableSize << "\n";
    }
    
    // Output in array format for easy copying
    cout << "\nArray format:\n";
    cout << "const Magic " << (bishopMode ? "BishopMagics" : "RookMagics") << "[64] = {\n";
    for (int sq = 0; sq < 64; sq++) {
        auto &m = best[sq];
        cout << "    {0x" << hex << setw(16) << setfill('0') << m.number << ", " << dec << m.shift << ", " << m.tableSize << "}";
        if (sq < 63) cout << ",";
        cout << "\n";
    }
    cout << "};\n";

    return 0;
}