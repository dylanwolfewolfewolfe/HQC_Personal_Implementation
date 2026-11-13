#include "arith_reference.h"
#include <string.h>


static int bit_get(const uint64_t *a, size_t i) 
    { return (int)((a[i/64] >> (i%64)) & 1u); 
        }

static void bit_flip(uint64_t *a, size_t i) 
    { a[i/64] ^= (uint64_t)1 << (i%64); 
        }

void poly_xor(uint64_t *out, const uint64_t *a, const uint64_t *b, size_t words) {
    for (size_t i = 0; i < words; ++i) out[i] = a[i] ^ b[i];
}

void poly_mul_schoolbook(uint64_t *out, const uint64_t *a, const uint64_t *b, size_t bits) {
    memset(out, 0, ((bits+63)/64)*sizeof(uint64_t));
    for (size_t i = 0; i < bits; ++i)
        if (bit_get(a, i))
            for (size_t j = 0; j < bits; ++j)
                if (bit_get(b, j)) bit_flip(out, (i+j)%bits);
}

void poly_mul_sparse(uint64_t *out, const uint64_t *dense,
                     const size_t *positions, size_t weight, size_t bits) {
    memset(out, 0, ((bits+63)/64)*sizeof(uint64_t));
    for (size_t k = 0; k < weight; ++k)
        for (size_t j = 0; j < bits; ++j)
            if (bit_get(dense, j)) bit_flip(out, (positions[k]+j)%bits);
}
