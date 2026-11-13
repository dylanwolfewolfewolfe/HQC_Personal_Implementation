#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H
#include "parameters.h"
#define HQC_MESSAGE_BYTES VEC_K_SIZE_BYTES
#define HQC_PUBLIC_BYTES PUBLIC_KEY_BYTES
#define HQC_SECRET_BYTES SECRET_KEY_BYTES
/* Direct PKE ciphertext: u || v (without KEM hash or salt). */
#define HQC_PKE_CIPHERTEXT_BYTES (VEC_N_SIZE_BYTES + VEC_N1N2_SIZE_BYTES)
#endif
