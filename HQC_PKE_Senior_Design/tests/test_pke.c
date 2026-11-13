#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "my_hqc.h"
#include "arith_reference.h"

int main(void) {
    uint8_t pk[HQC_PUBLIC_BYTES], sk[HQC_SECRET_BYTES];
    uint8_t msg[HQC_MESSAGE_BYTES] = "Hello HQC!";
    uint8_t decoded[HQC_MESSAGE_BYTES] = {0};
    uint8_t ct[HQC_PKE_CIPHERTEXT_BYTES];
    uint64_t a[2] = {0b100101,0}, b[2] = {0b101000,0};
    uint64_t school[2] = {0}, sparse[2] = {0};
    size_t positions[] = {3,5};



    poly_mul_schoolbook(school, a, b, 64);
    poly_mul_sparse(sparse, a, positions, 2, 64);
    
    if (school[0] != sparse[0]) { puts("Arithmetic FAILED"); return 1; }
    puts("Arithmetic PASS");
    for (int i = 0; i < 5; ++i) {
        if (my_hqc_keygen(pk, sk) || my_hqc_encrypt(ct, msg, pk) || my_hqc_decrypt(decoded, ct, sk)) {
            puts("HQC operation failed"); return 1;
        }
        if (memcmp(msg, decoded, HQC_MESSAGE_BYTES)) {
            puts("HQC FAILED"); return 1;
        }

        printf("HQC-128 KeyGen/Encrypt/Decrypt round trip %d: PASS\n", i+1);
    }

    printf("Message bytes: %d | Ciphertext bytes: %d\n", HQC_MESSAGE_BYTES, HQC_PKE_CIPHERTEXT_BYTES);
    return 0;
}
