#ifndef MY_HQC_H
#define MY_HQC_H
#include <stdint.h>
#include "config.h"

/* Educational HQC Round-4 PKE; not an authenticated, CCA-secure encryption API. */
int my_hqc_keygen(uint8_t pk[HQC_PUBLIC_BYTES], uint8_t sk[HQC_SECRET_BYTES]);
int my_hqc_encrypt(uint8_t ct[HQC_PKE_CIPHERTEXT_BYTES],
                   const uint8_t msg[HQC_MESSAGE_BYTES],
                   const uint8_t pk[HQC_PUBLIC_BYTES]);
int my_hqc_decrypt(uint8_t msg[HQC_MESSAGE_BYTES],
                   const uint8_t ct[HQC_PKE_CIPHERTEXT_BYTES],
                   const uint8_t sk[HQC_SECRET_BYTES]);
#endif
