#include "my_hqc.h"
#include "os_random.h"
#include "vector.h"
#include "gf2x.h"
#include "code.h"
#include "parsing.h"
#include "shake_prng.h"
#include <immintrin.h>
#include <string.h>


int my_hqc_keygen(uint8_t pk[HQC_PUBLIC_BYTES], uint8_t sk[HQC_SECRET_BYTES]) {
    uint8_t entropy[48], secret_seed[SEED_BYTES], public_seed[SEED_BYTES];
    seedexpander_state secret_rng, public_rng;
    __m256i x[VEC_N_256_SIZE_64 / 4] = {0};
    __m256i y[VEC_N_256_SIZE_64 / 4] = {0};
    __m256i h[VEC_N_256_SIZE_64 / 4] = {0};
    __m256i product[VEC_N_256_SIZE_64 / 4] = {0};
    uint64_t syndrome[VEC_N_256_SIZE_64] = {0};

    if (os_random(entropy, sizeof entropy)) return -1;
    shake_prng_init(entropy, NULL, sizeof entropy, 0);
    shake_prng(secret_seed, sizeof secret_seed);
    shake_prng(public_seed, sizeof public_seed);
    seedexpander_init(&secret_rng, secret_seed, sizeof secret_seed);
    seedexpander_init(&public_rng, public_seed, sizeof public_seed);

    vect_set_random_fixed_weight(&secret_rng, x, PARAM_OMEGA);
    vect_set_random_fixed_weight(&secret_rng, y, PARAM_OMEGA);
    vect_set_random(&public_rng, (uint64_t *)h);

    /* s = x + h*y in F2[X] / (X^n - 1). */
    vect_mul(product, h, y);
    vect_add(syndrome, (const uint64_t *)x, (const uint64_t *)product, VEC_N_256_SIZE_64);
    hqc_public_key_to_string(pk, public_seed, syndrome);
    hqc_secret_key_to_string(sk, secret_seed, pk);
    return 0;
}

int my_hqc_encrypt(uint8_t ct[HQC_PKE_CIPHERTEXT_BYTES],
                   const uint8_t msg[HQC_MESSAGE_BYTES],
                   const uint8_t pk[HQC_PUBLIC_BYTES]) {
    uint8_t theta[SEED_BYTES];
    seedexpander_state rng;
    __m256i h[VEC_N_256_SIZE_64 / 4] = {0};
    __m256i s[VEC_N_256_SIZE_64 / 4] = {0};
    __m256i r1[VEC_N_256_SIZE_64 / 4] = {0};
    __m256i r2[VEC_N_256_SIZE_64 / 4] = {0};
    __m256i error[VEC_N_256_SIZE_64 / 4] = {0};
    __m256i product[VEC_N_256_SIZE_64 / 4] = {0};
    uint64_t u[VEC_N_256_SIZE_64] = {0};
    uint64_t v[VEC_N1N2_256_SIZE_64] = {0};
    uint64_t expanded[VEC_N_256_SIZE_64] = {0};
    uint64_t tmp[VEC_N_256_SIZE_64] = {0};
    uint64_t message_words[VEC_K_SIZE_64] = {0};

    if (os_random(theta, sizeof theta)) return -1;
    memcpy(message_words, msg, HQC_MESSAGE_BYTES);
    seedexpander_init(&rng, theta, sizeof theta);
    hqc_public_key_from_string((uint64_t *)h, (uint64_t *)s, pk);
    vect_set_random_fixed_weight(&rng, r1, PARAM_OMEGA_R);
    vect_set_random_fixed_weight(&rng, r2, PARAM_OMEGA_R);
    vect_set_random_fixed_weight(&rng, error, PARAM_OMEGA_E);

    /* u = r1 + h*r2 */
    vect_mul(product, h, r2);
    vect_add(u, (const uint64_t *)r1, (const uint64_t *)product, VEC_N_256_SIZE_64);

    /* v = Encode(m) + s*r2 + e; only the first n1*n2 bits are transmitted. */
    code_encode(v, message_words);
    vect_resize(expanded, PARAM_N, v, PARAM_N1N2);
    vect_mul(product, s, r2);
    vect_add(tmp, expanded, (const uint64_t *)product, VEC_N_256_SIZE_64);
    vect_add(expanded, tmp, (const uint64_t *)error, VEC_N_256_SIZE_64);
    vect_resize(v, PARAM_N1N2, expanded, PARAM_N);

    memcpy(ct, u, VEC_N_SIZE_BYTES);
    memcpy(ct + VEC_N_SIZE_BYTES, v, VEC_N1N2_SIZE_BYTES);
    return 0;
}

int my_hqc_decrypt(uint8_t msg[HQC_MESSAGE_BYTES],
                   const uint8_t ct[HQC_PKE_CIPHERTEXT_BYTES],
                   const uint8_t sk[HQC_SECRET_BYTES]) {
    __m256i x[VEC_N_256_SIZE_64 / 4] = {0};
    __m256i y[VEC_N_256_SIZE_64 / 4] = {0};
    uint8_t pk[HQC_PUBLIC_BYTES] = {0};
    __m256i u[VEC_N_256_SIZE_64 / 4] = {0};
    uint64_t v[VEC_N1N2_256_SIZE_64] = {0};
    __m256i product[VEC_N_256_SIZE_64 / 4] = {0};
    uint64_t expanded[VEC_N_256_SIZE_64] = {0};
    uint64_t noisy[VEC_N_256_SIZE_64] = {0};
    uint64_t message_words[VEC_K_SIZE_64] = {0};

    memcpy(u, ct, VEC_N_SIZE_BYTES);
    memcpy(v, ct + VEC_N_SIZE_BYTES, VEC_N1N2_SIZE_BYTES);
    hqc_secret_key_from_string(x, y, pk, sk);

    /* Decode(v + u*y); + is XOR in the binary ring. */
    vect_resize(expanded, PARAM_N, v, PARAM_N1N2);
    vect_mul(product, u, y);
    vect_add(noisy, expanded, (const uint64_t *)product, VEC_N_256_SIZE_64);
    code_decode(message_words, noisy);
    memcpy(msg, message_words, HQC_MESSAGE_BYTES);
    return 0;
}
