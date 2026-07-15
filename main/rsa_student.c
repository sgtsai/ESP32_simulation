#include "rsa_baseline.h"

static uint64_t modular_multiply_u64(uint64_t a, uint64_t b, uint64_t modulus)
{
    uint64_t result = 0;
    a %= modulus;

    while (b > 0) {
        if ((b & 1U) != 0U) {
            result = (result + a) % modulus;
        }
        a = (a + a) % modulus;
        b >>= 1U;
    }

    return result;
}

uint64_t rsa_encrypt_u64_student(uint64_t plaintext, uint64_t exponent, uint64_t modulus)
{
    if (modulus == 0U) {
        return 0U;
    }

    uint64_t result = 1U % modulus;
    uint64_t base = plaintext % modulus;

    while (exponent > 0U) {
        if ((exponent & 1U) != 0U) {
            result = modular_multiply_u64(result, base, modulus);
        }
        base = modular_multiply_u64(base, base, modulus);
        exponent >>= 1U;
    }

    return result;
}
