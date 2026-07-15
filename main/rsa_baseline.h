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
int rsa_extended_selftest(uint32_t *checksum);
int rsa_extended_benchmark(uint32_t rounds, int64_t *elapsed_us_out, uint32_t *checksum_out);
#endif
