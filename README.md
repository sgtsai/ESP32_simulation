# ESP32 RSA Optimization Assignment

This is a student optimization problem for ESP32. The program computes:

```text
ciphertext = plaintext^e mod n
```

Students start with a correct but slow implementation. Their job is to make
`main/rsa_student.c` faster without changing the required behavior.

## Goal

Optimize this function:

```c
uint64_t rsa_encrypt_u64_student(uint64_t plaintext,
                                 uint64_t exponent,
                                 uint64_t modulus);
```

The starter `main/rsa_student.c` is intentionally the same algorithm as
`main/rsa_baseline.c`. Students should improve only the student version.

## Run Environment

The project can be built and tested with:

- ESP-IDF
- Docker
- QEMU
- the host-side checker in `tools/`

Students should inspect the repository files and tool documentation to find
the exact commands.

Expected successful output includes:

```text
TEST ... PASS
BENCH_BASELINE ...
BENCH_STUDENT ...
RESULT correctness=PASS
SCORE correctness_points=... speed_points=... total_points=...
```

## What Students May Change

Students may edit:

- `main/rsa_student.c`
- helper files added under `main/`
- build settings, if documented in the report

Students should not change:

- `main/rsa_baseline.c`
- `main/app_main.c`
- public test vectors
- function signatures
- scoring scripts, unless the instructor allows it

## Correctness Rules

A valid solution must:

- produce the correct ciphertext for every test
- be deterministic
- handle edge cases such as `plaintext >= n`, `e = 1`, and small moduli
- avoid undefined behavior
- keep the baseline implementation unchanged

Correctness is required before performance points count.

## Performance Score

The judge ranks correct submissions by total time:

```text
total_time_us = public_time_us + hidden_time_us
```

Lower is better.

The public time comes from the visible benchmark. Hidden tests may use
different values, larger workloads, and repeated runs. Do not hard-code the
public answers.

## Project Layout

```text
.
|-- CMakeLists.txt
|-- Dockerfile
|-- main/
|   |-- CMakeLists.txt
|   |-- app_main.c
|   |-- rsa_baseline.c
|   |-- rsa_student.c
|   `-- rsa_baseline.h
|-- scripts/
|   |-- qemu_run.sh
|   `-- setup_from_github.sh
`-- tools/
    `-- score_host.py
```

## Scoring Criteria, 100 Points

The score is based only on correctness and speed.

Submissions must build and run in the assigned environment. A submission that
does not build receives 0 points.

### 1. Correctness, 60 Points

- 25 pts: all public tests pass
- 20 pts: all hidden tests pass
- 10 pts: edge cases work, including `plaintext >= n`, `e = 1`, and small moduli
- 5 pts: repeated runs produce the same results

If any correctness test fails, the speed score is 0.

### 2. Speed, 40 Points

Compute total time:

```text
baseline_total_us = public_baseline_us + hidden_baseline_us
student_total_us  = public_student_us  + hidden_student_us
```

Then compute speedup:

```text
speedup = baseline_total_us / student_total_us
```

Then compute speed points:

```text
speed_points = min(40, max(0, 8 * (speedup - 1)))
```

Examples:

- same speed as baseline: `speedup = 1.0`, speed score = 0
- 2x faster than baseline: `speedup = 2.0`, speed score = 8
- 3x faster than baseline: `speedup = 3.0`, speed score = 16
- 6x faster than baseline: `speedup = 6.0`, speed score = 40
- slower than baseline: speed score = 0

If any correctness test fails, the speed score is 0.

A submission needs all correctness points and at least 6x speedup to receive
100 total points.

## Suggested Report

Include:

- optimization summary
- before and after benchmark table
- correctness output
- test environment
- exact commands used
- tradeoffs or limitations

This assignment uses toy RSA values for teaching. They are not secure and must
not be used for real cryptography.
