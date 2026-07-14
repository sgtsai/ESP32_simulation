#pragma once

#include <stdint.h>

typedef struct {
    uint64_t plaintext;
    uint64_t exponent;
    uint64_t modulus;
    uint64_t expected;
} rsa_test_vector_t;

uint64_t rsa_encrypt_u64_baseline(uint64_t plaintext, uint64_t exponent, uint64_t modulus);
uint64_t rsa_encrypt_u64_student(uint64_t plaintext, uint64_t exponent, uint64_t modulus);
#ifdef RSA_ENABLE_REFANS
uint64_t rsa_encrypt_u64_refans(uint64_t plaintext, uint64_t exponent, uint64_t modulus);
#endif
int rsa_encrypt_u64_hw(uint64_t plaintext, uint64_t exponent, uint64_t modulus, uint64_t *ciphertext);
