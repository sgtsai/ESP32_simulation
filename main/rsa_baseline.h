#pragma once

#include <stdint.h>

typedef struct {
    uint64_t plaintext;
    uint64_t exponent;
    uint64_t modulus;
    uint64_t expected;
} rsa_test_vector_t;

uint64_t rsa_encrypt_u64(uint64_t plaintext, uint64_t exponent, uint64_t modulus);
