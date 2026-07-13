#!/usr/bin/env python3
"""Host-side mock correctness and timing check for the ESP32 RSA lab."""

from __future__ import annotations

import statistics
import time


VECTORS = [
    (65, 17, 3233, 2790),
    (42, 17, 3233, 2557),
    (1234, 17, 3233, 2183),
    (9999, 17, 3233, 391),
    (1234567, 65537, 100160063, 14546671),
    (987654321, 65537, 100160063, 80367779),
    (100160064, 1, 100160063, 1),
    (37, 5, 97, 15),
]


def modular_multiply_u64(a: int, b: int, modulus: int) -> int:
    result = 0
    a %= modulus
    while b > 0:
        if b & 1:
            result = (result + a) % modulus
        a = (a + a) % modulus
        b >>= 1
    return result


def rsa_encrypt_u64(plaintext: int, exponent: int, modulus: int) -> int:
    if modulus == 0:
        return 0

    result = 1 % modulus
    base = plaintext % modulus
    while exponent > 0:
        if exponent & 1:
            result = modular_multiply_u64(result, base, modulus)
        base = modular_multiply_u64(base, base, modulus)
        exponent >>= 1
    return result


def run_correctness() -> bool:
    ok = True
    for index, (plaintext, exponent, modulus, expected) in enumerate(VECTORS):
        got = rsa_encrypt_u64(plaintext, exponent, modulus)
        passed = got == expected
        print(f"TEST {index}: got={got} expected={expected} {'PASS' if passed else 'FAIL'}")
        ok = ok and passed
    return ok


def run_benchmark(samples: int = 7) -> None:
    timings = []
    checksum = 0
    rounds = 200

    for _ in range(samples):
        start = time.perf_counter()
        local_checksum = 0
        for round_index in range(rounds):
            for plaintext, exponent, modulus, _ in VECTORS:
                local_checksum ^= rsa_encrypt_u64(plaintext + round_index, exponent, modulus)
        elapsed = time.perf_counter() - start
        timings.append(elapsed)
        checksum ^= local_checksum

    print(f"BENCH samples={samples} rounds={rounds} median_s={statistics.median(timings):.6f}")
    print(f"BENCH min_s={min(timings):.6f} max_s={max(timings):.6f} checksum={checksum}")


def main() -> int:
    ok = run_correctness()
    run_benchmark()
    print(f"RESULT correctness={'PASS' if ok else 'FAIL'}")
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
