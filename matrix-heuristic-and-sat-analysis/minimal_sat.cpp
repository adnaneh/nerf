/******************************************************************
 * Variant B: hand-written CMS5 model — 2-round version (no Z3)
 ******************************************************************/
#include <cryptominisat5/cryptominisat.h>
#include <array>
#include <vector>
#include <cstdint>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>

using namespace CMSat;

/* ---------- helpers ----------------------------------------------------- */
static inline Lit new_lit(SATSolver& S) {
    S.new_var();
    return Lit(S.nVars() - 1, false);
}
static inline Lit neg(const Lit& l) { return ~l; }

/* exactly-one using a Sinz sequential counter - O(n) clauses, O(n) fresh vars */
static void exactly_one_seq(SATSolver& S, const std::vector<Lit>& x)
{
    if (x.size() <= 1) return;
    std::vector<Lit> a(x.size() - 1);
    for (auto& v : a) v = new_lit(S);

    S.add_clause(x);                         //  ≥1
    S.add_clause({neg(x[0]),     a[0]});     //  x0 → a0
    for (size_t i = 1; i + 1 < x.size(); ++i) {
        S.add_clause({neg(a[i-1]), a[i]});   //  ai-1 → ai
        S.add_clause({neg(x[i]),   a[i]});   //  xi  → ai
        S.add_clause({neg(x[i]), neg(a[i-1])}); // xi  → ¬ai-1
    }
    S.add_clause({neg(x.back()), neg(a.back())}); // xn-1 → ¬an-1
}

/* ---------- encode one S-box byte --------------------------------------- */
static void encode_sbox_byte(
        SATSolver&               S,
        const std::array<Lit, 8>& in_bits,
        std::array<Lit, 8>&       out_bits,
        const uint8_t            confusion[256])
{
    /* 1. selector literals s_v  (v = 0..255) */
    std::vector<Lit> sel(256);
    for (int v = 0; v < 256; ++v) sel[v] = new_lit(S);
    exactly_one_seq(S, sel);                   // exactly one active selector

    /* 2. link selectors to input pattern */
    for (int v = 0; v < 256; ++v) {
        for (int b = 0; b < 8; ++b) {
            bool bit = (v >> b) & 1;
            S.add_clause({ neg(sel[v]), bit ? in_bits[b] : neg(in_bits[b]) });
        }
    }

    /* 3. create 8 output literals  */
    for (int b = 0; b < 8; ++b) out_bits[b] = new_lit(S);

    /* 4. tie output bits to the constant S-box entry confusion[v] */
    for (int v = 0; v < 256; ++v)
        for (int b = 0; b < 8; ++b) {
            bool bit = (confusion[v] >> b) & 1;
            S.add_clause({ neg(sel[v]), bit ? out_bits[b] : neg(out_bits[b]) });
        }
}

/* ---------- parity-diffusion row: CMS has native XOR clauses ------------ */
static void xor_row(
        SATSolver&                               S,
        const std::vector<std::array<Lit, 8>>&   in_bytes,
        const uint32_t                           diffusion[32],
        int                                      row_idx,
        std::array<Lit, 8>&                      out_byte)
{
    for (int b = 0; b < 8; ++b) out_byte[b] = new_lit(S);

    /* one XOR equation per bit position */
    for (int bit = 0; bit < 8; ++bit) {
        std::vector<Lit> lhs;
        uint32_t row = diffusion[row_idx];
        while (row) {
            int k = __builtin_ctz(row);
            lhs.push_back(in_bytes[k][bit]);
            row &= row - 1;
        }
        lhs.push_back(out_byte[bit]);   // XOR(inputs) == out_bit == 0
        S.add_xor_clause(lhs, /*rhs=*/false);   // CMS native XOR
    }
}

/* ---------- driver ------------------------------------------------------ */
int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    SATSolver S;
    S.set_num_threads(14);  // Use all physical cores

    /* tiny AES-like confusion table (example only) */
    std::array<uint8_t, 256> confusion = {
        0xac,0xd1,0x25,0x94,0x1f,0xb3,0x33,0x28,0x7c,0x2b,0x17,0xbc,0xf6,0xb0,0x55,0x5d,
        0x8f,0xd2,0x48,0xd4,0xd3,0x78,0x62,0x1a,0x02,0xf2,0x01,0xc9,0xaa,0xf0,0x83,0x71,
        0x72,0x4b,0x6a,0xe8,0xe9,0x42,0xc0,0x53,0x63,0x66,0x13,0x4a,0xc1,0x85,0xcf,0x0c,
        0x24,0x76,0xa5,0x6e,0xd7,0xa1,0xec,0xc6,0x04,0xc2,0xa2,0x5c,0x81,0x92,0x6c,0xda,
        0xc6,0x86,0xba,0x4d,0x39,0xa0,0x0e,0x8c,0x8a,0xd0,0xfe,0x59,0x96,0x49,0xe6,0xea,
        0x69,0x30,0x52,0x1c,0xe0,0xb2,0x05,0x9b,0x10,0x03,0xa8,0x64,0x51,0x97,0x02,0x09,
        0x8e,0xad,0xf7,0x36,0x47,0xab,0xce,0x7f,0x56,0xca,0x00,0xe3,0xed,0xf1,0x38,0xd8,
        0x26,0x1c,0xdc,0x35,0x91,0x43,0x2c,0x74,0xb4,0x61,0x9d,0x5e,0xe9,0x4c,0xbf,0x77,
        0x16,0x1e,0x21,0x1d,0x2d,0xa9,0x95,0xb8,0xc3,0x8d,0xf8,0xdb,0x34,0xe1,0x84,0xd6,
        0x0b,0x23,0x4e,0xff,0x3c,0x54,0xa7,0x78,0xa4,0x89,0x33,0x6d,0xfb,0x79,0x27,0xc4,
        0xf9,0x40,0x41,0xdf,0xc5,0x82,0x93,0xdd,0xa6,0xef,0xcd,0x8d,0xa3,0xae,0x7a,0xb6,
        0x2f,0xfd,0xbd,0xe5,0x98,0x66,0xf3,0x4f,0x57,0x88,0x90,0x9c,0x0a,0x50,0xe7,0x15,
        0x7b,0x58,0xbc,0x07,0x68,0x3a,0x5f,0xee,0x32,0x9f,0xeb,0xcc,0x18,0x8b,0xe2,0x57,
        0xb7,0x49,0x37,0xde,0xf5,0x99,0x67,0x5b,0x3b,0xbb,0x3d,0xb5,0x2d,0x19,0x2e,0x0d,
        0x93,0xfc,0x7e,0x06,0x08,0xbe,0x3f,0xd9,0x2a,0x70,0x9a,0xc8,0x7d,0xd8,0x46,0x65,
        0x22,0xf4,0xb9,0xa2,0x6f,0x12,0x1b,0x14,0x45,0xc7,0x87,0x31,0x60,0x29,0xf7,0x73
    };

    /* simple involutory 32×32 binary diffusion matrix (row-major) */
    uint32_t diffusion[32] = {
        0xf26cb481,0x16a5dc92,0x3c5ba924,0x79b65248,0x2fc64b18,0x615acd29,0xc3b59a42,0x976b2584,
        0x6cf281b4,0xa51692dc,0x5b3c24a9,0xb6794852,0xc62f184b,0x5a6129cd,0xb5c3429a,0x6b978425,
        0xb481f26c,0xdc9216a5,0xa9243c5b,0x524879b6,0x4b182fc6,0xcd29615a,0x9a42c3b5,0x2584976b,
        0x81b46cf2,0x92dca516,0x24a95b3c,0x4852b679,0x184bc62f,0x29cd5a61,0x429ab5c3,0x84256b97
    };

    /* (A) declare plaintext bits -------------------------------------------------- */
    std::vector<std::array<Lit, 8>> x(32);   // round-0 state (plaintext)
    for (auto& byte : x)
        for (auto& bit : byte) bit = new_lit(S);

    /* ----- S‑box #1:  d0[k] = S(x[k])  ----- */
    std::vector<std::array<Lit,8>> d0(32);
    for (int k=0;k<32;++k)
        encode_sbox_byte(S, x[k], d0[k], confusion.data());

    /* ----- diffusion #1:  y0[i] = XOR M[i,*]·d0[*]  ----- */
    std::vector<std::array<Lit,8>> y0(32);
    for (int i=0;i<32;++i)
        xor_row(S, d0, diffusion, i, y0[i]);

    /* ----- S‑box #2:  d1[k] = S(y0[k])  ----- */
    std::vector<std::array<Lit,8>> d1(32);
    for (int k=0;k<32;++k)
        encode_sbox_byte(S, y0[k], d1[k], confusion.data());

    /* --------------------------------------------------------------------------- */
    /*              generate **known** 2-round ciphertext                          */
    /* --------------------------------------------------------------------------- */
    uint8_t plaintext[32];          // random test vector
    for (int i = 0; i < 32; ++i) plaintext[i] = rand() & 0xFF;

    /* --- round 1 --- */
    uint8_t state[32];          memcpy(state, plaintext, 32);
    for (int j = 0; j < 32; ++j) state[j] = confusion[state[j]];   // S-box
    uint8_t c1[32];             memset(c1, 0, 32);
    for (int j = 0; j < 32; ++j)
        for (int k = 0; k < 32; ++k)
            if (diffusion[j] >> k & 1)
                c1[j] ^= state[k];                                 // diffusion

    /* --- round 2 --- */
    uint8_t temp2[32];          memcpy(temp2, c1, 32);
    for (int j = 0; j < 32; ++j) temp2[j] = confusion[temp2[j]];   // S-box
    uint8_t c2[32];             memset(c2, 0, 32);
    for (int j = 0; j < 32; ++j)
        for (int k = 0; k < 32; ++k)
            if (diffusion[j] >> k & 1)
                c2[j] ^= temp2[k];                                 // diffusion

    /* print known test vector ---------------------------------------------------- */
    std::cout << "Original plaintext: ";
    for (int i = 0; i < 32; ++i) std::printf("%02x", plaintext[i]);
    std::cout << "\nCiphertext after 2 rounds: ";
    for (int i = 0; i < 32; ++i) std::printf("%02x", c2[i]);
    std::cout << "\n\nConstraining SAT model to the above ciphertext...\n";

    /* ----- diffusion #2: constrain to *known* c2[i]  ----- */
    for (int i=0;i<32;++i) {
        std::array<Lit,8> tmp;
        xor_row(S, d1, diffusion, i, tmp);
        for (int b=0;b<8;++b) {
            bool bit = (c2[i]>>b)&1;
            S.add_clause({ bit ?  tmp[b] : neg(tmp[b]) });   // unit
        }
    }

    /* (G) solve ------------------------------------------------------------------ */
    std::cout << "CNF has " << S.nVars() << " variables\n";
    std::cout << "Solving SAT problem (2-round inversion)...\n";
    auto start = std::chrono::high_resolution_clock::now();
    lbool result = S.solve();
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "SAT solving took: " << duration.count() << " µs (" 
              << duration.count() / 1000.0 << " ms)\n";

    if (result != l_True) {
        std::cerr << "UNSAT/UNKNOWN — could not invert 2 rounds!\n";
        return 1;
    }

    /* (H) extract plaintext ------------------------------------------------------ */
    const auto& m = S.get_model();
    uint8_t recovered[32]{};

    std::cout << "Recovered plaintext: ";
    for (int j = 0; j < 32; ++j) {
        uint8_t byte = 0;
        for (int b = 0; b < 8; ++b)
            if (m[x[j][b].var()] == l_True) byte |= (1u << b);
        recovered[j] = byte;
        std::printf("%02x", byte);
    }
    std::cout << '\n';

    /* (I) verify ----------------------------------------------------------------- */
    // encrypt recovered plaintext for 2 rounds and compare
    uint8_t test[32];           memcpy(test, recovered, 32);

    // round 1
    for (int j = 0; j < 32; ++j) test[j] = confusion[test[j]];
    uint8_t t1[32]{};
    for (int j = 0; j < 32; ++j)
        for (int k = 0; k < 32; ++k)
            if (diffusion[j] >> k & 1) t1[j] ^= test[k];

    // round 2
    for (int j = 0; j < 32; ++j) t1[j] = confusion[t1[j]];
    uint8_t t2[32]{};
    for (int j = 0; j < 32; ++j)
        for (int k = 0; k < 32; ++k)
            if (diffusion[j] >> k & 1) t2[j] ^= t1[k];

    bool ok = std::memcmp(t2, c2, 32) == 0;
    std::cout << (ok ? "✅ Verification SUCCESS!" : "❌ Verification FAILED!") << '\n';

    return ok ? 0 : 2;
}