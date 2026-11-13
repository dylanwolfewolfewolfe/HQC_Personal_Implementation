#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "my_hqc.h"
#ifdef __x86_64__
#include <x86intrin.h>
static uint64_t now(void) { _mm_lfence(); return __rdtsc(); }
static uint64_t stop(void) { unsigned aux; uint64_t t=__rdtscp(&aux); _mm_lfence(); return t; }
#else
#error "This benchmark uses the x86 timestamp counter"
#endif
#define RUNS 100


int main(void) {
    uint8_t pk[HQC_PUBLIC_BYTES], sk[HQC_SECRET_BYTES];
    uint8_t ct[HQC_PKE_CIPHERTEXT_BYTES];
    uint8_t msg[HQC_MESSAGE_BYTES] = "Benchmark HQC";
    uint8_t recovered[HQC_MESSAGE_BYTES] = {0};
    uint64_t sum_key=0,sum_enc=0,sum_dec=0;

    for (int i=0; i<RUNS; ++i) {
        uint64_t a=now(); if(my_hqc_keygen(pk,sk))return 1; uint64_t b=stop(); sum_key += b-a;
        a=now(); if(my_hqc_encrypt(ct,msg,pk))return 1; b=stop(); sum_enc += b-a;
        a=now(); if(my_hqc_decrypt(recovered,ct,sk))return 1; b=stop(); sum_dec += b-a;
        if(memcmp(msg,recovered,HQC_MESSAGE_BYTES)) { puts("Decrypt failed"); return 1; }
    }

    
    printf("HQC-128 direct PKE, %d runs (average TSC ticks)\n", RUNS);
    printf("Key Generation: %.1f\n",(double)sum_key/RUNS);
    printf("Encryption:     %.1f\n",(double)sum_enc/RUNS);
    printf("Decryption:     %.1f\n",(double)sum_dec/RUNS);
    return 0;
}
