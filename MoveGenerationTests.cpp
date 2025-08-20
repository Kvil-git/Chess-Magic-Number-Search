#include <iostream>
#include <vector>
#include <cstdint>
#include <array>
#include <bitset>
#include <assert.h>
#include <random>
#include <chrono>

using namespace std;

using Bitboard = uint64_t;

// Magic number structure
struct MagicEntry {
    uint64_t magic;
    int shift;
    int tableSize;
};

constexpr MagicEntry RookMagics[64] = {
    {0x0080064000201081, 52, 4096},
    {0x0040021001406000, 53, 2048},
    {0x2080200070001880, 53, 2048},
    {0x8100100100542088, 53, 2048},
    {0x8e00042008120110, 53, 2048},
    {0x4b00082400021100, 53, 2048},
    {0x02000200040800b1, 53, 2048},
    {0x0500028608402100, 52, 4096},
    {0x9800800299214000, 53, 2048},
    {0x0402002200810050, 54, 1024},
    {0x0005001045002001, 54, 1024},
    {0x0004801800d00280, 54, 1024},
    {0x002900080101504c, 54, 1024},
    {0x100d000c00880300, 54, 1024},
    {0x2023808001003a00, 54, 1024},
    {0x0101000182224100, 53, 2048},
    {0x0004808000214000, 53, 2048},
    {0x0011020049220080, 54, 1024},
    {0x6288220040108203, 54, 1024},
    {0x0222818008025002, 54, 1024},
    {0x4004008080040800, 54, 1024},
    {0x0500808012000400, 54, 1024},
    {0x4012050100020004, 54, 1024},
    {0x00000a0001840445, 53, 2048},
    {0x4200800180284002, 53, 2048},
    {0x001080a100400500, 54, 1024},
    {0x0402500480200480, 54, 1024},
    {0x2100100280080081, 54, 1024},
    {0x0048010100110c08, 54, 1024},
    {0x6000040080800200, 54, 1024},
    {0x0226088400010210, 54, 1024},
    {0x0018802680014100, 53, 2048},
    {0x3000804000801024, 53, 2048},
    {0x8110022001404008, 54, 1024},
    {0x0800500080806000, 54, 1024},
    {0x0880081042002200, 54, 1024},
    {0xa800850091000800, 54, 1024},
    {0x0101120080800400, 54, 1024},
    {0x0000100804000122, 54, 1024},
    {0x0004004286000c01, 53, 2048},
    {0x0028400280228000, 53, 2048},
    {0x0300c00081010020, 54, 1024},
    {0x4020001000858020, 54, 1024},
    {0x1002000810420020, 54, 1024},
    {0x03820020180e0010, 54, 1024},
    {0x0404040002008080, 54, 1024},
    {0x3000aa1810040015, 54, 1024},
    {0x4100010064920004, 53, 2048},
    {0x6100204008801180, 53, 2048},
    {0x010c2010004000c0, 54, 1024},
    {0x8040402003001900, 54, 1024},
    {0x0302001008224200, 54, 1024},
    {0x4201000800308700, 54, 1024},
    {0x0010800400020080, 54, 1024},
    {0x2004500208031400, 54, 1024},
    {0x0000800100005280, 53, 2048},
    {0x00a02202450080b2, 52, 4096},
    {0x808a220700401282, 53, 2048},
    {0x2200200040090011, 53, 2048},
    {0x2201000409201001, 53, 2048},
    {0x1002010c08102042, 53, 2048},
    {0x1d01000804000201, 53, 2048},
    {0x01000802011000cc, 53, 2048},
    {0x800081002088c402, 52, 4096}
};

constexpr MagicEntry BishopMagics[64] = {
    {0x4040082704028010, 58, 64},
    {0x000218c813004087, 59, 32},
    {0x40849c0082004061, 59, 32},
    {0xd012408102000840, 59, 32},
    {0x00820a1000000000, 59, 32},
    {0x0201092031000000, 59, 32},
    {0x4002021082090403, 59, 32},
    {0x0001004804040a41, 58, 64},
    {0x000a292208520402, 59, 32},
    {0x008014051c250e03, 59, 32},
    {0x00501c0802014800, 59, 32},
    {0x830104104a002002, 59, 32},
    {0x0100908820201030, 59, 32},
    {0x00c0044108400214, 59, 32},
    {0x10007400a208600d, 59, 32},
    {0x0000012101105001, 59, 32},
    {0x0204082254840802, 59, 32},
    {0x1004003004088420, 59, 32},
    {0x0461120802006200, 57, 128},
    {0x0104830802004100, 57, 128},
    {0x2404028480a00000, 57, 128},
    {0x4402000088040220, 57, 128},
    {0x0082000103092189, 59, 32},
    {0x00820c0041442c00, 59, 32},
    {0x001018a4414a0400, 59, 32},
    {0x0002060650040800, 59, 32},
    {0xc0a8880010004550, 57, 128},
    {0x4008080042620020, 55, 512},
    {0x3201001081004001, 55, 512},
    {0x004040806510100c, 57, 128},
    {0x6048028201040101, 59, 32},
    {0x1000408382060100, 59, 32},
    {0x0011201005220400, 59, 32},
    {0x090082100c200404, 59, 32},
    {0x3020402800100d40, 57, 128},
    {0x0082008400020120, 55, 512},
    {0x00040100700c00c0, 55, 512},
    {0x8210204080011004, 57, 128},
    {0x0f07020600041100, 59, 32},
    {0x0003040020408210, 59, 32},
    {0xea0c023842002c10, 59, 32},
    {0x0494012410014a01, 59, 32},
    {0x2017004022003004, 57, 128},
    {0x0002401414000800, 57, 128},
    {0x4404400091000200, 57, 128},
    {0x0624008401008810, 57, 128},
    {0x0d0210060a000280, 59, 32},
    {0x0104440042000040, 59, 32},
    {0x0002080212d08000, 59, 32},
    {0x0001410828220024, 59, 32},
    {0x000000c200d01c20, 59, 32},
    {0x00000000840c2000, 59, 32},
    {0x000264080b040040, 59, 32},
    {0x0018604431020470, 59, 32},
    {0x0140022881010414, 59, 32},
    {0x508810118a004981, 59, 32},
    {0x8400841101012012, 58, 64},
    {0x2020008205b00400, 59, 32},
    {0xc010071042080400, 59, 32},
    {0x00044080649c0400, 59, 32},
    {0x0090502051120202, 59, 32},
    {0x2002002002060a00, 59, 32},
    {0xf010905050008284, 59, 32},
    {0x00848804b8018100, 58, 64}

};

// Attack tables
array<vector<Bitboard>, 64> RookAttackTable;
array<vector<Bitboard>, 64> BishopAttackTable;

// Masks for rook and bishop
array<Bitboard, 64> RookMasks;
array<Bitboard, 64> BishopMasks;

// Utility functions
template<class T> inline T BIT(const T &x) {
    return (T(1) << x);
}

template<class T> inline bool IsBitSet(const T &object, int bitIndex) {
    return (object >> bitIndex) & 1;
}

template<class T> inline void SetBit(T &object, int bitIndex) {
    object |= BIT<T>(bitIndex);
}

// Count bits in a bitboard (popcount)
int CountBits(Bitboard b) {
    return bitset<64>(b).count();
}

// Print a bitboard in a chessboard format
void PrintBitboard(Bitboard bb, const string& title = "") {
    if (!title.empty()) {
        cout << title << ":\n";
    }
    
    for (int row = 7; row >= 0; row--) {
        cout << row + 1 << " ";
        for (int col = 0; col < 8; col++) {
            int square = row * 8 + col;
            cout << (IsBitSet(bb, square) ? "x " : ". ");
        }
        cout << "\n";
    }
    cout << "  a b c d e f g h\n\n";
}

// Generate rook moves mask (excluding edges)
Bitboard GenerateRookMovesMask(int sqr) {
    Bitboard output = 0;
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

Bitboard GenerateBishopMovesMask(int sqr) {
    Bitboard output = 0;
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
Bitboard GenerateRookAttacks(int sq, Bitboard blockers) {
    Bitboard attacks = 0;
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
Bitboard GenerateBishopAttacks(int sq, Bitboard blockers) {
    Bitboard attacks = 0;
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

// Initialize all masks
void InitializeMasks() {
    for (int sq = 0; sq < 64; sq++) {
        RookMasks[sq] = GenerateRookMovesMask(sq);
        BishopMasks[sq] = GenerateBishopMovesMask(sq);
    }
}

// Initialize attack tables
void InitializeAttackTables() {
    // Initialize rook attack tables
    for (int sq = 0; sq < 64; sq++) {
        if (RookMagics[sq].shift == 64) {
            // Skip if magic not found for this square
            continue;
        }
        
        Bitboard mask = RookMasks[sq];
        int tableSize = RookMagics[sq].tableSize;
        RookAttackTable[sq].resize(tableSize, 0);
        
        // Generate all possible blocker configurations using Carry-Rippler method
        Bitboard blockers = 0;
        do {
            // Calculate index using magic multiplication
            int index = (blockers * RookMagics[sq].magic) >> RookMagics[sq].shift;
            
            // Generate and store attacks for this blocker configuration
            if (index >= 0 && index < tableSize) {
                RookAttackTable[sq][index] = GenerateRookAttacks(sq, blockers);
            }
            
            // Get next subset of blockers
            blockers = (blockers - mask) & mask;
        } while (blockers != 0);
    }
    
    // Initialize bishop attack tables
    for (int sq = 0; sq < 64; sq++) {
        if (BishopMagics[sq].shift == 64) {
            // Skip if magic not found for this square
            continue;
        }
        
        Bitboard mask = BishopMasks[sq];
        int tableSize = BishopMagics[sq].tableSize;
        BishopAttackTable[sq].resize(tableSize, 0);
        
        // Generate all possible blocker configurations using Carry-Rippler method
        Bitboard blockers = 0;
        do {
            // Calculate index using magic multiplication
            int index = (blockers * BishopMagics[sq].magic) >> BishopMagics[sq].shift;
            
            // Generate and store attacks for this blocker configuration
            if (index >= 0 && index < tableSize) {
                BishopAttackTable[sq][index] = GenerateBishopAttacks(sq, blockers);
            }
            
            // Get next subset of blockers
            blockers = (blockers - mask) & mask;
        } while (blockers != 0);
    }
}

// Get rook attacks using magic bitboards
Bitboard GetRookAttacks(int sq, Bitboard occupancy) {
    // Get relevant blockers (only those on the mask)
    Bitboard blockers = occupancy & RookMasks[sq];
    
    // Calculate index using magic multiplication
    int index = (blockers * RookMagics[sq].magic) >> RookMagics[sq].shift;
    
    // Return precomputed attacks with bounds checking
    if (index >= 0 && index < static_cast<int>(RookAttackTable[sq].size())) {
        return RookAttackTable[sq][index];
    }
    
    // Fallback if index is out of bounds
    return GenerateRookAttacks(sq, occupancy);
}

// Get bishop attacks using magic bitboards
Bitboard GetBishopAttacks(int sq, Bitboard occupancy) {
    // Get relevant blockers (only those on the mask)
    Bitboard blockers = occupancy & BishopMasks[sq];
    
    // Calculate index using magic multiplication
    int index = (blockers * BishopMagics[sq].magic) >> BishopMagics[sq].shift;
    
    // Return precomputed attacks with bounds checking
    if (index >= 0 && index < static_cast<int>(BishopAttackTable[sq].size())) {
        return BishopAttackTable[sq][index];
    }
    
    // Fallback if index is out of bounds
    return GenerateBishopAttacks(sq, occupancy);
}

// Get queen attacks (combination of rook and bishop)
Bitboard GetQueenAttacks(int sq, Bitboard occupancy) {
    return GetRookAttacks(sq, occupancy) | GetBishopAttacks(sq, occupancy);
}

// Validate magic numbers
bool ValidateMagic(int sq, const MagicEntry& magic, bool isBishop) {
    Bitboard mask = isBishop ? BishopMasks[sq] : RookMasks[sq];
    vector<Bitboard> table(magic.tableSize, 0);
    
    Bitboard blockers = 0;
    do {
        int index = (blockers * magic.magic) >> magic.shift;
        Bitboard attacks = isBishop ? GenerateBishopAttacks(sq, blockers) 
                                   : GenerateRookAttacks(sq, blockers);
        
        if (table[index] != 0 && table[index] != attacks) {
            cerr << "Collision detected for square " << sq 
                 << " at index " << index 
                 << " with magic " << hex << magic.magic << dec
                 << " and shift " << magic.shift << endl;
            return false; // Collision detected
        }
        table[index] = attacks;
        
        blockers = (blockers - mask) & mask;
    } while (blockers != 0);
    
    return true;
}

void TestSlidingPieceMoveGeneration() {
    cout << "Running sliding piece move generation tests..." << endl;

    // Test 1: Empty board rook moves from a1 (square 0)
    {
        Bitboard occ = 0;
        Bitboard attacks = GetRookAttacks(0, occ);
        // Should cover 14 squares (7 up, 7 right)
        assert(CountBits(attacks) == 14);
        assert(IsBitSet(attacks, 8));  // a2
        assert(IsBitSet(attacks, 7));  // h1
        assert(!IsBitSet(attacks, 0)); // itself not included
    }

    // Test 2: Empty board bishop moves from d4 (square 27)
    {
        Bitboard occ = 0;
        Bitboard attacks = GetBishopAttacks(27, occ);
        // Bishop from d4 should see 13 squares diagonally
        assert(CountBits(attacks) == 13);
        assert(IsBitSet(attacks, 18)); // c3
        assert(IsBitSet(attacks, 36)); // e5
        assert(IsBitSet(attacks, 54)); // g7
        assert(!IsBitSet(attacks, 27)); // itself not included
    }

    // Test 3: Rook blocked on e4 (square 28)
    {
        Bitboard occ = 0;
        SetBit(occ, 36); // e5 (blocker)
        SetBit(occ, 20); // e3 (blocker)
        Bitboard attacks = GetRookAttacks(28, occ);

        // Should include e5 and e3, but not beyond
        assert(IsBitSet(attacks, 36));
        assert(IsBitSet(attacks, 20));
        assert(!IsBitSet(attacks, 44)); // e6 should be blocked
        assert(!IsBitSet(attacks, 12)); // e2 should be blocked
    }

    // Test 4: Bishop blocked on c1 (square 2)
    {
        Bitboard occ = 0;
        SetBit(occ, 9);  // d2 (blocker)
        Bitboard attacks = GetBishopAttacks(2, occ);

        // Should include d2 but not beyond
        assert(IsBitSet(attacks, 9));
        assert(!IsBitSet(attacks, 16)); // e3 should be blocked
    }

    // Test 5: Queen attacks = rook | bishop
    {
        Bitboard occ = 0;
        int sq = 27; // d4
        Bitboard queen = GetQueenAttacks(sq, occ);
        Bitboard combined = GetRookAttacks(sq, occ) | GetBishopAttacks(sq, occ);
        assert(queen == combined);
    }

    cout << "All sliding piece tests passed!" << endl;
}

// ... (your existing code)

void TestRandomizedSlidingAttacks(int iterations = 100000) {
    std::mt19937_64 rng(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);
    
    cout << "Running randomized sliding attack tests (" << iterations << " iterations)..." << endl;
    
    for (int i = 0; i < iterations; i++) {
        // Generate random occupancy
        Bitboard occupancy = dist(rng);
        
        for (int sq = 0; sq < 64; sq++) {
            // Test rook attacks
            Bitboard rookMagic = GetRookAttacks(sq, occupancy);
            Bitboard rookBrute = GenerateRookAttacks(sq, occupancy);
            
            if (rookMagic != rookBrute) {
                cerr << "Rook attack mismatch at square " << sq 
                     << " with occupancy: 0x" << hex << occupancy << dec << endl;
                cerr << "Magic: 0x" << hex << rookMagic << dec << ", Brute: 0x" << hex << rookBrute << dec << endl;
                assert(false);
            }
            
            // Test bishop attacks
            Bitboard bishopMagic = GetBishopAttacks(sq, occupancy);
            Bitboard bishopBrute = GenerateBishopAttacks(sq, occupancy);
            
            if (bishopMagic != bishopBrute) {
                cerr << "Bishop attack mismatch at square " << sq 
                     << " with occupancy: 0x" << hex << occupancy << dec << endl;
                cerr << "Magic: 0x" << hex << bishopMagic << dec << ", Brute: 0x" << hex << bishopBrute << dec << endl;
                assert(false);
            }
            
            // Test queen attacks (rook | bishop)
            Bitboard queenMagic = GetQueenAttacks(sq, occupancy);
            Bitboard queenBrute = rookBrute | bishopBrute;
            
            if (queenMagic != queenBrute) {
                cerr << "Queen attack mismatch at square " << sq 
                     << " with occupancy: 0x" << hex << occupancy << dec << endl;
                cerr << "Magic: 0x" << hex << queenMagic << dec << ", Brute: 0x" << hex << queenBrute << dec << endl;
                assert(false);
            }
        }
        
        // Progress indicator
        if ((i + 1) % 1000 == 0) {
            cout << "Completed " << (i + 1) << " iterations..." << endl;
        }
    }
    
    cout << "All randomized tests passed!" << endl;
}

void TestEdgeCases() {
    cout << "Testing edge cases..." << endl;
    
    // Test with all squares occupied
    Bitboard allOccupied = ~0ULL;
    for (int sq = 0; sq < 64; sq++) {
        Bitboard rookMagic = GetRookAttacks(sq, allOccupied);
        Bitboard rookBrute = GenerateRookAttacks(sq, allOccupied);
        assert(rookMagic == rookBrute);
        
        Bitboard bishopMagic = GetBishopAttacks(sq, allOccupied);
        Bitboard bishopBrute = GenerateBishopAttacks(sq, allOccupied);
        assert(bishopMagic == bishopBrute);
    }
    
    // Test with no squares occupied
    Bitboard noneOccupied = 0;
    for (int sq = 0; sq < 64; sq++) {
        Bitboard rookMagic = GetRookAttacks(sq, noneOccupied);
        Bitboard rookBrute = GenerateRookAttacks(sq, noneOccupied);
        assert(rookMagic == rookBrute);
        
        Bitboard bishopMagic = GetBishopAttacks(sq, noneOccupied);
        Bitboard bishopBrute = GenerateBishopAttacks(sq, noneOccupied);
        assert(bishopMagic == bishopBrute);
    }
    
    // Test with alternating pattern occupancy
    Bitboard checkerboard = 0xAAAAAAAAAAAAAAAA;
    for (int sq = 0; sq < 64; sq++) {
        Bitboard rookMagic = GetRookAttacks(sq, checkerboard);
        Bitboard rookBrute = GenerateRookAttacks(sq, checkerboard);
        assert(rookMagic == rookBrute);
        
        Bitboard bishopMagic = GetBishopAttacks(sq, checkerboard);
        Bitboard bishopBrute = GenerateBishopAttacks(sq, checkerboard);
        assert(bishopMagic == bishopBrute);
    }
    
    cout << "All edge case tests passed!" << endl;
}

void TestSpecificProblemSquares() {
    cout << "Testing specific problem squares..." << endl;
    
    // Create random number generator for this function
    std::mt19937_64 rng(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);
    
    // Test squares that are known to be problematic for magic bitboards
    vector<int> problemSquares = {0, 7, 56, 63}; // corners
    vector<int> centerSquares = {27, 28, 35, 36}; // center
    
    for (int sq : problemSquares) {
        for (int i = 0; i < 1000; i++) {
            Bitboard occupancy = dist(rng);
            
            Bitboard rookMagic = GetRookAttacks(sq, occupancy);
            Bitboard rookBrute = GenerateRookAttacks(sq, occupancy);
            assert(rookMagic == rookBrute);
            
            Bitboard bishopMagic = GetBishopAttacks(sq, occupancy);
            Bitboard bishopBrute = GenerateBishopAttacks(sq, occupancy);
            assert(bishopMagic == bishopBrute);
        }
    }
    
    for (int sq : centerSquares) {
        for (int i = 0; i < 1000; i++) {
            Bitboard occupancy = dist(rng);
            
            Bitboard rookMagic = GetRookAttacks(sq, occupancy);
            Bitboard rookBrute = GenerateRookAttacks(sq, occupancy);
            assert(rookMagic == rookBrute);
            
            Bitboard bishopMagic = GetBishopAttacks(sq, occupancy);
            Bitboard bishopBrute = GenerateBishopAttacks(sq, occupancy);
            assert(bishopMagic == bishopBrute);
        }
    }
    
    cout << "All problem square tests passed!" << endl;
}
void PrintRandomTestCases(int numCases = 5) {
    std::mt19937_64 rng(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);
    
    cout << "\n=== Printing " << numCases << " Random Test Cases ===\n" << endl;
    
    for (int i = 0; i < numCases; i++) {
        // Generate random square and occupancy
        int square = dist(rng) % 64;
        Bitboard occupancy = dist(rng);
        
        // Get attacks
        Bitboard rookAttacks = GetRookAttacks(square, occupancy);
        Bitboard bishopAttacks = GetBishopAttacks(square, occupancy);
        Bitboard queenAttacks = GetQueenAttacks(square, occupancy);
        
        // Convert square to chess notation
        char file = 'a' + (square % 8);
        int rank = (square / 8) + 1;
        
        cout << "Test Case " << (i + 1) << ": Square " << file << rank << " (index " << square << ")\n";
        cout << "========================================\n";
        
        // Print occupancy
        PrintBitboard(occupancy, "Occupancy");
        
        // Print rook attacks
        PrintBitboard(rookAttacks, "Rook Attacks");
        
        // Print bishop attacks
        PrintBitboard(bishopAttacks, "Bishop Attacks");
        
        // Print queen attacks
        PrintBitboard(queenAttacks, "Queen Attacks");
        
        // Verify with brute force
        Bitboard rookBrute = GenerateRookAttacks(square, occupancy);
        Bitboard bishopBrute = GenerateBishopAttacks(square, occupancy);
        
        if (rookAttacks == rookBrute && bishopAttacks == bishopBrute) {
            cout << "✓ Magic bitboards match brute force calculation!\n";
        } else {
            cout << "✗ ERROR: Magic bitboards don't match brute force!\n";
        }
        
        cout << "\n" << string(50, '=') << "\n\n";
    }
}

// Add this to your main function after all other tests
int main() {
    cout << "Initializing magic bitboards..." << endl;
    
    // Initialize masks and attack tables
    InitializeMasks();
    InitializeAttackTables();
    
    cout << "Initialization complete." << endl;
    
    // Validate all magic numbers
    cout << "Validating magic numbers..." << endl;
    for (int sq = 0; sq < 64; sq++) {
        if (!ValidateMagic(sq, RookMagics[sq], false)) {
            cerr << "Rook magic validation failed for square " << sq << endl;
            return 1;
        }
        if (!ValidateMagic(sq, BishopMagics[sq], true)) {
            cerr << "Bishop magic validation failed for square " << sq << endl;
            return 1;
        }
    }
    cout << "All magic numbers validated successfully." << endl;
    
    // Run basic tests
    TestSlidingPieceMoveGeneration();
    
    // Run comprehensive randomized tests
    TestEdgeCases();
    TestSpecificProblemSquares();
    TestRandomizedSlidingAttacks(100000); // 100,000 iterations
    
    // Print some visual test cases to enjoy the success
    PrintRandomTestCases(5);
    
    cout << "All tests passed! Your magic bitboard implementation is working correctly." << endl;
    
    return 0;
}
