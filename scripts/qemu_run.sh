#!/usr/bin/env bash
set -euo pipefail

idf.py set-target esp32
idf.py build

if idf.py --help | grep -q "qemu"; then
    idf.py qemu monitor
else
    echo "idf.py qemu is not available in this ESP-IDF image."
    echo "Use python tools/score_host.py for a local mock benchmark."
    exit 2
fi
