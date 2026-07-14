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
        const uint64_t got_baseline = rsa_encrypt_u64_baseline(v->plaintext, v->exponent, v->modulus);
        const uint64_t got_student = rsa_encrypt_u64_student(v->plaintext, v->exponent, v->modulus);
#ifdef RSA_ENABLE_REFANS
        const uint64_t got_refans = rsa_encrypt_u64_refans(v->plaintext, v->exponent, v->modulus);
#endif
        uint64_t got_hw = 0;
        const int hw_ret = rsa_encrypt_u64_hw(v->plaintext, v->exponent, v->modulus, &got_hw);
        const bool pass = (got_baseline == v->expected) &&
                          (got_student == v->expected) &&
#ifdef RSA_ENABLE_REFANS
                          (got_refans == v->expected) &&
#endif
                          (hw_ret == 0) &&
                          (got_hw == v->expected);

#ifdef RSA_ENABLE_REFANS
        printf("TEST %u: baseline=%" PRIu64 " student=%" PRIu64
               " refans=%" PRIu64 " hw=%" PRIu64
               " expected=%" PRIu64 " hw_ret=%d %s\n",
               (unsigned)i,
               got_baseline,
               got_student,
               got_refans,
               got_hw,
               v->expected,
               hw_ret,
               pass ? "PASS" : "FAIL");
#else
        printf("TEST %u: baseline=%" PRIu64 " student=%" PRIu64
               " hw=%" PRIu64 " expected=%" PRIu64 " hw_ret=%d %s\n",
               (unsigned)i,
               got_baseline,
               got_student,
               got_hw,
               v->expected,
               hw_ret,
               pass ? "PASS" : "FAIL");
#endif

        ok = ok && pass;
    }

    return ok;
}

static uint64_t run_baseline_benchmark(int64_t *elapsed_us_out)
{
    uint64_t checksum = 0;
    const int rounds = 200;
    const size_t count = sizeof(k_vectors) / sizeof(k_vectors[0]);
    const int64_t start_us = esp_timer_get_time();

    for (int round = 0; round < rounds; ++round) {
        for (size_t i = 0; i < count; ++i) {
            const rsa_test_vector_t *v = &k_vectors[i];
            checksum ^= rsa_encrypt_u64_baseline(v->plaintext + (uint64_t)round,
                                                 v->exponent,
                                                 v->modulus);
        }
    }

    *elapsed_us_out = esp_timer_get_time() - start_us;
    printf("BENCH_BASELINE rounds=%d vectors=%u elapsed_us=%" PRId64 " checksum=%" PRIu64 "\n",
           rounds,
           (unsigned)count,
           *elapsed_us_out,
           checksum);

    return checksum;
}

static uint64_t run_student_benchmark(int64_t *elapsed_us_out)
{
    uint64_t checksum = 0;
    const int rounds = 200;
    const size_t count = sizeof(k_vectors) / sizeof(k_vectors[0]);
    const int64_t start_us = esp_timer_get_time();

    for (int round = 0; round < rounds; ++round) {
        for (size_t i = 0; i < count; ++i) {
            const rsa_test_vector_t *v = &k_vectors[i];
            checksum ^= rsa_encrypt_u64_student(v->plaintext + (uint64_t)round,
                                                v->exponent,
                                                v->modulus);
        }
    }

    *elapsed_us_out = esp_timer_get_time() - start_us;
    printf("BENCH_STUDENT rounds=%d vectors=%u elapsed_us=%" PRId64 " checksum=%" PRIu64 "\n",
           rounds,
           (unsigned)count,
           *elapsed_us_out,
           checksum);

    return checksum;
}

static uint64_t run_hardware_benchmark(int64_t *elapsed_us_out)
{
    uint64_t checksum = 0;
    const int rounds = 200;
    const size_t count = sizeof(k_vectors) / sizeof(k_vectors[0]);
    const int64_t start_us = esp_timer_get_time();

    for (int round = 0; round < rounds; ++round) {
        for (size_t i = 0; i < count; ++i) {
            const rsa_test_vector_t *v = &k_vectors[i];
            uint64_t ciphertext = 0;
            if (rsa_encrypt_u64_hw(v->plaintext + (uint64_t)round,
                                   v->exponent,
                                   v->modulus,
                                   &ciphertext) == 0) {
                checksum ^= ciphertext;
            }
        }
    }

    *elapsed_us_out = esp_timer_get_time() - start_us;
    printf("BENCH_HW rounds=%d vectors=%u elapsed_us=%" PRId64 " checksum=%" PRIu64 "\n",
           rounds,
           (unsigned)count,
           *elapsed_us_out,
           checksum);

    return checksum;
}

#ifdef RSA_ENABLE_REFANS
static uint64_t run_refans_benchmark(int64_t *elapsed_us_out)
{
    uint64_t checksum = 0;
    const int rounds = 200;
    const size_t count = sizeof(k_vectors) / sizeof(k_vectors[0]);
    const int64_t start_us = esp_timer_get_time();

    for (int round = 0; round < rounds; ++round) {
        for (size_t i = 0; i < count; ++i) {
            const rsa_test_vector_t *v = &k_vectors[i];
            checksum ^= rsa_encrypt_u64_refans(v->plaintext + (uint64_t)round,
                                               v->exponent,
                                               v->modulus);
        }
    }

    *elapsed_us_out = esp_timer_get_time() - start_us;
    printf("BENCH_REFANS rounds=%d vectors=%u elapsed_us=%" PRId64 " checksum=%" PRIu64 "\n",
           rounds,
           (unsigned)count,
           *elapsed_us_out,
           checksum);

    return checksum;
}
#endif

void app_main(void)
{
    printf("ESP32 RSA hardware-accelerated optimization assignment\n");

    const bool ok = run_correctness_tests();
    int64_t baseline_elapsed_us = 0;
    int64_t student_elapsed_us = 0;
    int64_t hw_elapsed_us = 0;
#ifdef RSA_ENABLE_REFANS
    int64_t refans_elapsed_us = 0;
#endif
    const uint64_t baseline_checksum = run_baseline_benchmark(&baseline_elapsed_us);
    const uint64_t student_checksum = run_student_benchmark(&student_elapsed_us);
#ifdef RSA_ENABLE_REFANS
    const uint64_t refans_checksum = run_refans_benchmark(&refans_elapsed_us);
#endif
    const uint64_t hw_checksum = run_hardware_benchmark(&hw_elapsed_us);

#ifdef RSA_ENABLE_REFANS
    printf("RESULT correctness=%s baseline_us=%" PRId64 " student_us=%" PRId64
           " refans_us=%" PRId64 " hw_us=%" PRId64
           " baseline_checksum=%" PRIu64 " student_checksum=%" PRIu64
           " refans_checksum=%" PRIu64 " hw_checksum=%" PRIu64 "\n",
           ok ? "PASS" : "FAIL",
           baseline_elapsed_us,
           student_elapsed_us,
           refans_elapsed_us,
           hw_elapsed_us,
           baseline_checksum,
           student_checksum,
           refans_checksum,
           hw_checksum);
#else
    printf("RESULT correctness=%s baseline_us=%" PRId64 " student_us=%" PRId64
           " hw_us=%" PRId64 " baseline_checksum=%" PRIu64
           " student_checksum=%" PRIu64 " hw_checksum=%" PRIu64 "\n",
           ok ? "PASS" : "FAIL",
           baseline_elapsed_us,
           student_elapsed_us,
           hw_elapsed_us,
           baseline_checksum,
           student_checksum,
           hw_checksum);
#endif
}
