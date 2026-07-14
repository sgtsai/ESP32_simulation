# ESP32 RSA Optimization Assignment

This repository is a student testing problem for RSA encryption on an ESP32 target. Students compare a correct software baseline with an ESP32 hardware-accelerated path, then optimize and explain the performance difference while preserving correctness.

The project is designed to run in three ways:

- ESP32 hardware with ESP-IDF
- Docker for a reproducible ESP-IDF build environment
- QEMU mock run for students who do not have a board

## Assignment Goal

Optimize RSA encryption on ESP32:

```text
ciphertext = plaintext^e mod n
```

The baseline implementation in `main/rsa_baseline.c` uses straightforward square-and-multiply modular exponentiation. The hardware path in `main/rsa_hw_accel.c` uses `mbedtls_mpi_exp_mod()`. On ESP32 with ESP-IDF hardware MPI support enabled, ESP-IDF can route supported big-number operations through the ESP32 RSA/MPI hardware accelerator.

This repository includes `sdkconfig.defaults` with:

```text
CONFIG_MBEDTLS_HARDWARE_MPI=y
```

That option is intended to enable the mbedTLS big-number hardware acceleration path on ESP32 targets that support it.

## Environment Hints

Students should determine the exact commands themselves from the repository files and tool documentation.

If using local ESP-IDF:

- Find the official Espressif ESP-IDF GitHub repository.
- Use the ESP-IDF version implied by `Dockerfile`.
- Install the ESP32 toolchain.
- Export or open the ESP-IDF environment so `idf.py` is available.
- Configure the target chip as ESP32.
- Build this project from the repository root.
- Check the mbedTLS hardware MPI/RSA acceleration option in menu configuration.

If using Docker:

- Inspect `Dockerfile` to identify the base image and default working directory.
- Build the Docker image from this repository.
- Mount the project directory into the container at `/work`.
- Run ESP-IDF commands inside the container.
- Confirm that generated build files appear in the host `build/` directory.

## QEMU Mock Run Hints

The repository includes a QEMU helper script under `scripts/`. Students should inspect that script to learn:

- how the ESP32 target is configured
- how QEMU is launched
- why the emulator is stopped after a timeout
- how to change the timeout with an environment variable

QEMU support depends on the ESP-IDF Docker image version. If QEMU is unavailable, students should use the host-side checker in `tools/` for software correctness and mock timing.

The host checker does not emulate ESP32 hardware acceleration. Real hardware-accelerator timing should be measured from ESP32 firmware output on real hardware when possible.

Expected successful firmware output includes:

```text
TEST ... PASS
BENCH_BASELINE ...
BENCH_STUDENT ...
BENCH_HW ...
RESULT correctness=PASS
```

## Project Layout

```text
.
|-- CMakeLists.txt
|-- Dockerfile
|-- sdkconfig.defaults
|-- ESP32_RSA_Assignment_Guide.txt
|-- ESP32_RSA_Hardware_Accelerator_Note.txt
|-- main/
|   |-- CMakeLists.txt
|   |-- app_main.c
|   |-- rsa_baseline.c
|   |-- rsa_student.c
|   |-- rsa_hw_accel.c
|   |-- rsa_refans.c.example
|   `-- rsa_baseline.h
|-- scripts/
|   |-- qemu_run.sh
|   `-- setup_from_github.sh
`-- tools/
    `-- score_host.py
```

## What Students May Change

Students may edit:

- `main/rsa_student.c`
- `main/rsa_hw_accel.c`
- helper files added under `main/`
- build flags in `main/CMakeLists.txt`, if documented

Students should not change:

- `main/rsa_baseline.c`
- `main/app_main.c`
- expected test vectors
- `rsa_encrypt_u64_baseline` function signature
- `rsa_encrypt_u64_student` function signature
- `rsa_encrypt_u64_hw` function signature
- scoring scripts, unless the instructor explicitly permits it

Instructor-only reference answers may be kept in `main/rsa_refans.c`. That file is ignored by git and should not be distributed to students. The public `main/rsa_refans.c.example` shows the expected private-file shape.

## Correctness Requirements

An optimized solution must:

- produce exactly the expected ciphertext for every test vector
- handle all provided moduli and exponents
- avoid undefined behavior in integer arithmetic
- remain deterministic across repeated runs
- keep software and hardware results consistent
- keep the baseline implementation unchanged

The starter assignment uses 64-bit RSA-sized toy values so the optimization problem is easy to run in class, QEMU, and CI. These values are not cryptographically secure. They exist only to teach modular arithmetic, accelerator use, and benchmarking on embedded systems.

## Suggested Optimization Ideas

Start simple:

- remove unnecessary modulo operations
- reduce branches inside the exponentiation loop
- use faster modular multiplication
- precompute reusable values
- compare software baseline timing against the ESP32 hardware accelerator
- reduce conversion overhead around the hardware path
- compare compiler optimization flags
- measure stack and heap usage

Harder improvements:

- constant-time modular exponentiation
- fixed-window exponentiation
- Montgomery multiplication
- assembly-assisted multiplication
- careful use of ESP32 cycle counters
- benchmark methodology that separates setup time from encryption time

## Scoring Criteria, 100 Points

The rubric is ordered from easiest to hardest.

### 1. Build and Run, 10 Points

- 4 pts: project builds with ESP-IDF
- 3 pts: Docker build works
- 3 pts: QEMU or host mock run produces benchmark output

### 2. Correctness, 20 Points

- 10 pts: all public test vectors pass
- 5 pts: repeated runs are deterministic
- 5 pts: implementation handles edge cases such as `plaintext >= n`, `e = 1`, and small moduli

### 3. Code Quality, 10 Points

- 4 pts: code is readable and maintainable
- 3 pts: changes are well localized
- 2 pts: no unnecessary global state
- 1 pt: comments explain non-obvious math or ESP32-specific behavior

### 4. Hardware Accelerator Use, 15 Points

- 5 pts: hardware-accelerated path builds and runs
- 5 pts: hardware path produces the same ciphertexts as the software baseline
- 5 pts: student explains how ESP-IDF/mbedTLS reaches the ESP32 RSA/MPI accelerator

### 5. Performance Measurement, 15 Points

- 5 pts: student measures software baseline before changing it
- 5 pts: student measures hardware-accelerated timing separately
- 5 pts: benchmark result is reproducible across multiple runs

### 6. Advanced Optimization, 10 Points

- 4 pts: student reduces overhead around accelerator setup, conversion, or repeated work
- 3 pts: timing excludes logging and setup overhead where possible
- 3 pts: student explains software-vs-hardware tradeoffs with evidence

### 7. Embedded Awareness, 10 Points

- 4 pts: stack and heap usage are considered
- 3 pts: solution works in QEMU or on real ESP32 hardware
- 3 pts: report distinguishes QEMU/mock timing from real ESP32 accelerator timing

### 8. Security and Robustness, 5 Points

- 2 pts: avoids overflow-related undefined behavior
- 2 pts: considers timing side channels or constant-time tradeoffs
- 1 pt: validates assumptions about input ranges

### 9. Report and Reproducibility, 5 Points

- 3 pts: report includes commands, environment, and measured results
- 2 pts: report compares software baseline and hardware-accelerated versions clearly

## Submission Notes

A strong submission should include:

- short description of the optimization
- before/after benchmark table
- explanation of any tradeoffs
- confirmation that all correctness tests pass
- hardware, QEMU, or host-checker environment used
- exact commands the student used

## Instructor Notes

This assignment is intentionally scoped around toy RSA values. For a longer project, instructors can extend the same API to multi-precision integers, require real ESP32 hardware measurements, or compare with mbedTLS on larger operands.
