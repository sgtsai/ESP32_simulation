#!/usr/bin/env bash
set -euo pipefail

ESP_IDF_DIR="${ESP_IDF_DIR:-$HOME/esp/esp-idf}"
ESP_IDF_VERSION="${ESP_IDF_VERSION:-v5.3.2}"

if [ ! -d "$ESP_IDF_DIR/.git" ]; then
    mkdir -p "$(dirname "$ESP_IDF_DIR")"
    git clone --recursive https://github.com/espressif/esp-idf.git "$ESP_IDF_DIR"
fi

cd "$ESP_IDF_DIR"
git fetch --tags
git checkout "$ESP_IDF_VERSION"
git submodule update --init --recursive
./install.sh esp32

echo "Run this before building:"
echo ". \"$ESP_IDF_DIR/export.sh\""
