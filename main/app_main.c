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
        const bool pass = (got_baseline == v->expected) &&
                          (got_student == v->expected) &&
#ifdef RSA_ENABLE_REFANS
                          (got_refans == v->expected) &&
#endif
                          true;

#ifdef RSA_ENABLE_REFANS
        printf("TEST %u: baseline=%" PRIu64 " student=%" PRIu64
               " refans=%" PRIu64 " expected=%" PRIu64 " %s\n",
               (unsigned)i,
               got_baseline,
               got_student,
               got_refans,
               v->expected,
               pass ? "PASS" : "FAIL");
#else
        printf("TEST %u: baseline=%" PRIu64 " student=%" PRIu64
               " expected=%" PRIu64 " %s\n",
               (unsigned)i,
               got_baseline,
               got_student,
               v->expected,
               pass ? "PASS" : "FAIL");
#endif

        ok = ok && pass;
    }

#ifdef RSA_ENABLE_REFANS
    uint32_t big_checksum = 0;
    const int big_ret = rsa_extended_selftest(&big_checksum);
    printf("EXTENDED_TEST checksum=%u expected=%u ret=%d %s\n",
           (unsigned)big_checksum,
           (unsigned)1323555377U,
           big_ret,
           big_ret == 0 ? "PASS" : "FAIL");
    ok = ok && (big_ret == 0);
#endif

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

static int32_t compute_speed_points_x100(bool correct, int64_t baseline_us, int64_t student_us)
{
    if (!correct || baseline_us <= 0 || student_us <= 0 || student_us >= baseline_us) {
        return 0;
    }

    const int64_t saved_us = baseline_us - student_us;
    int64_t points_x100 = (800LL * saved_us + (student_us / 2)) / student_us;
    if (points_x100 > 4000) {
        points_x100 = 4000;
    }

    return (int32_t)points_x100;
}

static int64_t compute_speedup_x100(int64_t baseline_us, int64_t student_us)
{
    if (baseline_us <= 0 || student_us <= 0) {
        return 0;
    }

    return (100LL * baseline_us + (student_us / 2)) / student_us;
}

static void print_score(bool correct, int64_t baseline_us, int64_t student_us)
{
    const int32_t correctness_points = correct ? 60 : 0;
    const int32_t speed_points_x100 = compute_speed_points_x100(correct, baseline_us, student_us);
    const int32_t total_points_x100 = correctness_points * 100 + speed_points_x100;
    const int64_t speedup_x100 = compute_speedup_x100(baseline_us, student_us);
    const int32_t speed_points_whole = speed_points_x100 / 100;
    const int32_t speed_points_fraction = speed_points_x100 % 100;
    const int32_t total_points_whole = total_points_x100 / 100;
    const int32_t total_points_fraction = total_points_x100 % 100;

    printf("SCORE correctness_points=%" PRId32
           " speed_points=%" PRId32 ".%02" PRId32
           " total_points=%" PRId32 ".%02" PRId32
           " speedup=%" PRId64 ".%02" PRId64
           " baseline_total_us=%" PRId64 " student_total_us=%" PRId64 "\n",
           correctness_points,
           speed_points_whole,
           speed_points_fraction,
           total_points_whole,
           total_points_fraction,
           speedup_x100 / 100,
           speedup_x100 % 100,
           baseline_us,
           student_us);
}

#ifdef RSA_ENABLE_REFANS
static uint32_t run_extended_benchmark(int64_t *elapsed_us_out)
{
    uint32_t checksum = 0;
    const uint32_t rounds = 20;
    const int ret = rsa_extended_benchmark(rounds, elapsed_us_out, &checksum);

    printf("BENCH_EXTENDED rounds=%u elapsed_us=%" PRId64
           " checksum=%u ret=%d\n",
           (unsigned)rounds,
           *elapsed_us_out,
           (unsigned)checksum,
           ret);

    return checksum;
}

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
    printf("ESP32 RSA optimization assignment\n");

    const bool ok = run_correctness_tests();
    int64_t baseline_elapsed_us = 0;
    int64_t student_elapsed_us = 0;
#ifdef RSA_ENABLE_REFANS
    int64_t refans_elapsed_us = 0;
    int64_t extended_elapsed_us = 0;
#endif
    const uint64_t baseline_checksum = run_baseline_benchmark(&baseline_elapsed_us);
    const uint64_t student_checksum = run_student_benchmark(&student_elapsed_us);
    const bool checksum_ok = (baseline_checksum == student_checksum);
    const bool score_correct = ok && checksum_ok;
#ifdef RSA_ENABLE_REFANS
    const uint64_t refans_checksum = run_refans_benchmark(&refans_elapsed_us);
    const uint32_t extended_checksum = run_extended_benchmark(&extended_elapsed_us);
#endif

#ifdef RSA_ENABLE_REFANS
    printf("RESULT correctness=%s baseline_us=%" PRId64 " student_us=%" PRId64
           " refans_us=%" PRId64 " extended_us=%" PRId64
           " baseline_checksum=%" PRIu64 " student_checksum=%" PRIu64
           " checksum_match=%s refans_checksum=%" PRIu64 " extended_checksum=%u\n",
           score_correct ? "PASS" : "FAIL",
           baseline_elapsed_us,
           student_elapsed_us,
           refans_elapsed_us,
           extended_elapsed_us,
           baseline_checksum,
           student_checksum,
           checksum_ok ? "PASS" : "FAIL",
           refans_checksum,
           (unsigned)extended_checksum);
#else
    printf("RESULT correctness=%s baseline_us=%" PRId64 " student_us=%" PRId64
           " baseline_checksum=%" PRIu64 " student_checksum=%" PRIu64
           " checksum_match=%s\n",
           score_correct ? "PASS" : "FAIL",
           baseline_elapsed_us,
           student_elapsed_us,
           baseline_checksum,
           student_checksum,
           checksum_ok ? "PASS" : "FAIL");
#endif

    print_score(score_correct, baseline_elapsed_us, student_elapsed_us);
}
