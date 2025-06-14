#include <z3++.h>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <ctime>

using namespace z3;

// Original tables from the crack-me 
static const uint8_t confusion[256] = {
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

static const uint32_t diffusion[32] = {
    0xf26cb481,0x16a5dc92,0x3c5ba924,0x79b65248,0x2fc64b18,0x615acd29,0xc3b59a42,0x976b2584,
    0x6cf281b4,0xa51692dc,0x5b3c24a9,0xb6794852,0xc62f184b,0x5a6129cd,0xb5c3429a,0x6b978425,
    0xb481f26c,0xdc9216a5,0xa9243c5b,0x524879b6,0x4b182fc6,0xcd29615a,0x9a42c3b5,0x2584976b,
    0x81b46cf2,0x92dca516,0x24a95b3c,0x4852b679,0x184bc62f,0x29cd5a61,0x429ab5c3,0x84256b97
};

// Balanced XOR tree to keep AST depth logarithmic
static expr balanced_xor(context& ctx, std::vector<expr> v)
{
    if (v.empty()) return ctx.bv_val(0, 8);
    while (v.size() > 1) {
        std::vector<expr> next;
        for (size_t i = 0; i + 1 < v.size(); i += 2)
            next.push_back(v[i] ^ v[i+1]);
        if (v.size() & 1) next.push_back(v.back());
        v.swap(next);
    }
    return v[0];
}

// Forward exactly 1 round for reference
void forward_1_round(const uint8_t in[32], uint8_t out[32])
{
    memcpy(out, in, 32);
    uint8_t d[32];
    for (int j = 0; j < 32; ++j) { 
        d[j] = confusion[out[j]]; 
        out[j] = 0; 
    }
    for (int j = 0; j < 32; ++j)
        for (int k = 0; k < 32; ++k)
            if (diffusion[j] >> k & 1) 
                out[j] ^= d[k];
}

int main()
{
    // Generate random plaintext
    srand(time(NULL));
    uint8_t plaintext[32];
    for (int i = 0; i < 32; ++i) 
        plaintext[i] = rand() & 0xff;
    
    printf("Original plaintext: ");
    for (int i = 0; i < 32; ++i) 
        printf("%02x", plaintext[i]);
    printf("\n");
    
    // Generate c1 by running 1 round forward
    uint8_t c1[32];
    forward_1_round(plaintext, c1);
    
    printf("After 1 round (c1): ");
    for (int i = 0; i < 32; ++i) 
        printf("%02x", c1[i]);
    printf("\n");
    
    printf("Building constraints...\n");
    
    context ctx;
    solver s(ctx);
    expr_vector S0(ctx), S1(ctx);

    /* 32 byte variables for the pre-image */
    for (int j = 0; j < 32; ++j)
        S0.push_back(ctx.bv_const(("x" + std::to_string(j)).c_str(), 8));

    /* c1 is the one-round ciphertext you generated in C */
    for (int j = 0; j < 32; ++j)
        S1.push_back(ctx.bv_val(static_cast<unsigned>(c1[j]), 8));

    printf("Building shared S-box mux...\n");
    /* one shared mux ---------------------------------------------------- */
    expr x  = ctx.bv_const("x", 8);
    expr sb = ctx.bv_val(confusion[0], 8);
    for (unsigned i = 1; i < 256; ++i)
        sb = ite(x == ctx.bv_val(i, 8),
                 ctx.bv_val(confusion[i], 8),
                 sb);

    printf("Instantiating for 32 bytes via substitution...\n");
    /* instantiate for each byte ---------------------------------------- */
    std::vector<expr> d;
    d.reserve(32);
    for (int k = 0; k < 32; ++k) {
        expr_vector src(ctx), dst(ctx);
        src.push_back(x);
        dst.push_back(S0[k]);
        d.push_back(sb.substitute(src, dst));
    }
    
    printf("Adding XOR constraints...\n");

    /* --- add 32 equations S1[i] = XOR_{k} M[i,k] · d[k] -------------- */
    for (int i = 0; i < 32; ++i) {
        std::vector<expr> to_xor;
        uint32_t row = diffusion[i];
        while (row) {
            int k = __builtin_ctz(row);
            to_xor.push_back(d[k]);                   // just a pointer copy
            row &= row - 1;
        }
        s.add(S1[i] == balanced_xor(ctx, to_xor));
    }

    printf("Starting SAT solver (32 S-box outputs built once, reused in XORs)...\n");
    
    // Check if we're in the right logic
    std::cout << "goal has arrays? " 
              << (s.to_smt2().find("Array") != std::string::npos) << "\n";
    
    check_result r = s.check();
    
    if (r == sat) {
        printf("SAT: Found solution!\n");
        model m = s.get_model();
        printf("Recovered plaintext: ");
        for (int j = 0; j < 32; ++j)
            printf("%02x", m.eval(S0[j]).get_numeral_uint());
        printf("\n");
        
        // Verify
        uint8_t recovered[32];
        for (int j = 0; j < 32; ++j)
            recovered[j] = (uint8_t)m.eval(S0[j]).get_numeral_uint();
        
        uint8_t test[32];
        forward_1_round(recovered, test);
        
        if (memcmp(test, c1, 32) == 0) {
            printf("✅ Verification SUCCESS!\n");
        } else {
            printf("❌ Verification FAILED!\n");
        }
        
    } else if (r == unsat) {
        printf("UNSAT: No solution exists\n");
    } else {
        printf("UNKNOWN: Solver couldn't determine satisfiability\n");
        if (r == unknown) {
            std::cout << "Reason: " << s.reason_unknown() << "\n";
        }
    }
    
    return 0;
}