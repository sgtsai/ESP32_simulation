#include "rsa_baseline.h"

#include <stddef.h>
#include <stdint.h>

#include "mbedtls/bignum.h"

static void u64_to_be(uint64_t value, unsigned char out[8])
{
    for (size_t i = 0; i < 8; ++i) {
        out[7U - i] = (unsigned char)(value & 0xFFU);
        value >>= 8U;
    }
}

static uint64_t be_to_u64(const unsigned char in[8])
{
    uint64_t value = 0;

    for (size_t i = 0; i < 8; ++i) {
        value = (value << 8U) | (uint64_t)in[i];
    }

    return value;
}

int rsa_encrypt_u64_hw(uint64_t plaintext, uint64_t exponent, uint64_t modulus, uint64_t *ciphertext)
{
    if (ciphertext == NULL) {
        return -1;
    }

    if (modulus == 0U) {
        *ciphertext = 0U;
        return 0;
    }

    unsigned char plaintext_be[8];
    unsigned char exponent_be[8];
    unsigned char modulus_be[8];
    unsigned char ciphertext_be[8] = {0};

    mbedtls_mpi x;
    mbedtls_mpi e;
    mbedtls_mpi n;
    mbedtls_mpi c;

    mbedtls_mpi_init(&x);
    mbedtls_mpi_init(&e);
    mbedtls_mpi_init(&n);
    mbedtls_mpi_init(&c);

    u64_to_be(plaintext % modulus, plaintext_be);
    u64_to_be(exponent, exponent_be);
    u64_to_be(modulus, modulus_be);

    int ret = mbedtls_mpi_read_binary(&x, plaintext_be, sizeof(plaintext_be));
    if (ret == 0) {
        ret = mbedtls_mpi_read_binary(&e, exponent_be, sizeof(exponent_be));
    }
    if (ret == 0) {
        ret = mbedtls_mpi_read_binary(&n, modulus_be, sizeof(modulus_be));
    }
    if (ret == 0) {
        ret = mbedtls_mpi_exp_mod(&c, &x, &e, &n, NULL);
    }
    if (ret == 0) {
        ret = mbedtls_mpi_write_binary(&c, ciphertext_be, sizeof(ciphertext_be));
    }
    if (ret == 0) {
        *ciphertext = be_to_u64(ciphertext_be);
    }

    mbedtls_mpi_free(&c);
    mbedtls_mpi_free(&n);
    mbedtls_mpi_free(&e);
    mbedtls_mpi_free(&x);

    return ret;
}
