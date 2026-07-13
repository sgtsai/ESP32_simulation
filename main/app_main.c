#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "esp_timer.h"
#include "rsa_baseline.h"

static const rsa_test_vector_t k_vectors[] = {
    {65U, 17U, 3233U, 2790U},
    {42U, 17U, 3233U, 2557U},
    {1234U, 17U, 3233U, 2183U},
    {9999U, 17U, 3233U, 391U},
    {1234567U, 65537U, 100160063U, 14546671U},
    {987654321U, 65537U, 100160063U, 80367779U},
    {100160064U, 1U, 100160063U, 1U},
    {37U, 5U, 97U, 15U},
};

static bool run_correctness_tests(void)
{
    bool ok = true;
    const size_t count = sizeof(k_vectors) / sizeof(k_vectors[0]);

    for (size_t i = 0; i < count; ++i) {
        const rsa_test_vector_t *v = &k_vectors[i];
        const uint64_t got = rsa_encrypt_u64(v->plaintext, v->exponent, v->modulus);
        const bool pass = got == v->expected;

        printf("TEST %u: got=%" PRIu64 " expected=%" PRIu64 " %s\n",
               (unsigned)i,
               got,
               v->expected,
               pass ? "PASS" : "FAIL");

        ok = ok && pass;
    }

    return ok;
}

static uint64_t run_benchmark(void)
{
    uint64_t checksum = 0;
    const int rounds = 200;
    const size_t count = sizeof(k_vectors) / sizeof(k_vectors[0]);
    const int64_t start_us = esp_timer_get_time();

    for (int round = 0; round < rounds; ++round) {
        for (size_t i = 0; i < count; ++i) {
            const rsa_test_vector_t *v = &k_vectors[i];
            checksum ^= rsa_encrypt_u64(v->plaintext + (uint64_t)round,
                                        v->exponent,
                                        v->modulus);
        }
    }

    const int64_t elapsed_us = esp_timer_get_time() - start_us;
    printf("BENCH rounds=%d vectors=%u elapsed_us=%" PRId64 " checksum=%" PRIu64 "\n",
           rounds,
           (unsigned)count,
           elapsed_us,
           checksum);

    return checksum;
}

void app_main(void)
{
    printf("ESP32 RSA optimization assignment\n");

    const bool ok = run_correctness_tests();
    const uint64_t checksum = run_benchmark();

    printf("RESULT correctness=%s checksum=%" PRIu64 "\n", ok ? "PASS" : "FAIL", checksum);
}
