# ESP32 RSA Optimization Assignment

This repository is a student testing problem for optimizing RSA encryption on an ESP32 target. Students start from a correct but intentionally simple baseline, then improve performance while preserving correctness.

The project is designed to run in three ways:

- **ESP32 hardware** with ESP-IDF.
- **Docker** for a reproducible ESP-IDF build environment.
- **QEMU mock run** for students who do not have a board.

## Assignment Goal

Optimize RSA encryption on ESP32:

```text
ciphertext = plaintext^e mod n
```

The baseline implementation in `main/rsa_baseline.c` uses a straightforward square-and-multiply modular exponentiation algorithm. It is intentionally readable rather than fast. Students may optimize the implementation, but the public API and test behavior must remain compatible.

## Starting From the ESP-IDF GitHub Repo

If ESP-IDF is not already installed, clone the official GitHub repo:

```bash
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
git checkout v5.3.2
./install.sh esp32
. ./export.sh
```

Then return to this assignment repository and build:

```bash
idf.py set-target esp32
idf.py build
```

## Docker Setup

Docker is the easiest way to avoid local ESP-IDF setup differences.

```bash
docker build -t esp32-rsa-lab .
docker run --rm -it -v "$PWD:/work" esp32-rsa-lab idf.py build
```

On Windows PowerShell:

```powershell
docker run --rm -it -v "${PWD}:/work" esp32-rsa-lab idf.py build
```

## QEMU Mock Run

The QEMU workflow builds the ESP32 firmware and runs it in an emulator:

```bash
docker run --rm -it -v "$PWD:/work" esp32-rsa-lab bash scripts/qemu_run.sh
```

PowerShell:

```powershell
docker run --rm -it -v "${PWD}:/work" esp32-rsa-lab bash scripts/qemu_run.sh
```

QEMU support depends on the ESP-IDF Docker image version. If `idf.py qemu` is unavailable in your local image, use the host mock checker:

```bash
python tools/score_host.py
```

The host checker does not emulate ESP32 timing exactly, but it verifies correctness and gives a repeatable optimization signal.

## Project Layout

```text
.
├── CMakeLists.txt
├── Dockerfile
├── main/
│   ├── CMakeLists.txt
│   ├── app_main.c
│   ├── rsa_baseline.c
│   └── rsa_baseline.h
├── scripts/
│   ├── qemu_run.sh
│   └── setup_from_github.sh
└── tools/
    └── score_host.py
```

## What Students May Change

Students may edit:

- `main/rsa_baseline.c`
- helper files added under `main/`
- build flags in `main/CMakeLists.txt`, if documented

Students should not change:

- expected test vectors
- `rsa_encrypt_u64` function signature
- scoring scripts, unless the instructor explicitly permits it

## Correctness Requirements

An optimized solution must:

- produce exactly the expected ciphertext for every test vector
- handle all provided moduli and exponents
- avoid undefined behavior in integer arithmetic
- remain deterministic across repeated runs

The starter assignment uses 64-bit RSA-sized toy values so the optimization problem is easy to run in class, QEMU, and CI. These values are not cryptographically secure. They exist only to teach modular arithmetic optimization on embedded systems.

## Suggested Optimization Ideas

Start simple:

- remove unnecessary modulo operations
- reduce branches inside the exponentiation loop
- use faster modular multiplication
- precompute reusable values
- try Montgomery multiplication
- compare compiler optimization flags
- measure stack and heap usage

Harder improvements:

- constant-time modular exponentiation
- fixed-window exponentiation
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

### 3. Code Quality, 15 Points

- 5 pts: code is readable and maintainable
- 5 pts: changes are well localized
- 3 pts: no unnecessary global state
- 2 pts: comments explain non-obvious math or ESP32-specific behavior

### 4. Basic Performance Improvement, 15 Points

- 5 pts: student measures the baseline before changing it
- 5 pts: optimized code is at least 15% faster than baseline
- 5 pts: benchmark result is reproducible across multiple runs

### 5. Advanced Performance Improvement, 15 Points

- 5 pts: optimized code is at least 30% faster than baseline
- 5 pts: optimized code reduces expensive division or modulo operations
- 5 pts: student explains the chosen optimization strategy with evidence

### 6. Embedded Awareness, 10 Points

- 4 pts: stack and heap usage are considered
- 3 pts: solution works in QEMU or on real ESP32 hardware
- 3 pts: timing excludes logging and setup overhead where possible

### 7. Security and Robustness, 10 Points

- 4 pts: avoids overflow-related undefined behavior
- 3 pts: considers timing side channels or constant-time tradeoffs
- 3 pts: validates assumptions about input ranges

### 8. Report and Reproducibility, 5 Points

- 3 pts: report includes commands, environment, and measured results
- 2 pts: report compares baseline and optimized versions clearly

## Example Submission Notes

A strong submission should include:

- short description of the optimization
- before/after benchmark table
- explanation of any tradeoffs
- confirmation that all correctness tests pass
- hardware or QEMU environment used

## Instructor Notes

This assignment is intentionally scoped around toy RSA values. For a longer project, instructors can extend the same API to multi-precision integers or require use of ESP32 hardware cycle counters.
