#ifndef ARITH_REFERENCE_H
#define ARITH_REFERENCE_H
#include <stddef.h>
#include <stdint.h>
/* Simple, independent correctness references for small toy polynomials.
   NOT constant-time; do not use for sensitive production HQC. */
void poly_xor(uint64_t *out, const uint64_t *a, const uint64_t *b, size_t words);
void poly_mul_schoolbook(uint64_t *out, const uint64_t *a, const uint64_t *b, size_t bits);
void poly_mul_sparse(uint64_t *out, const uint64_t *dense,
                     const size_t *positions, size_t weight, size_t bits);
#endif
