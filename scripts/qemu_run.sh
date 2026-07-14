#!/usr/bin/env bash
set -euo pipefail

if [ ! -f sdkconfig ] || ! grep -q 'CONFIG_IDF_TARGET="esp32"' sdkconfig; then
    idf.py set-target esp32
fi

if idf.py --help | grep -q "qemu"; then
    timeout --foreground "${QEMU_TIMEOUT_SECONDS:-60}s" idf.py qemu || status=$?
    if [ "${status:-0}" -eq 124 ]; then
        echo "QEMU stopped after ${QEMU_TIMEOUT_SECONDS:-60} seconds. This is expected for the mock run."
    else
        exit "${status:-0}"
    fi
else
    idf.py build
    echo "idf.py qemu is not available in this ESP-IDF image."
    echo "Use python tools/score_host.py for a local mock benchmark."
    exit 2
fi
