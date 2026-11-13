#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
SRC=vendor/hqc_round4/src
FIPS=vendor/hqc_round4/lib/fips202
FLAGS=(-O3 -std=c11 -funroll-all-loops -flto -mavx -mavx2 -mbmi -mpclmul -I. -Isrc -I"$SRC" -I"$FIPS")
SOURCES=(src/my_hqc.c src/os_random.c src/arith_reference.c "$FIPS/fips202.c")
for f in vector reed_muller reed_solomon fft gf gf2x code parsing shake_ds shake_prng; do
  SOURCES+=("$SRC/$f.c")
done
LIBS=()
case "$(uname -s)" in MINGW*|MSYS*) LIBS+=(-lbcrypt) ;; esac
mkdir -p build
# Shared DLL used by Python, and native test / benchmark executables.
gcc "${FLAGS[@]}" -shared -o build/my_hqc.dll "${SOURCES[@]}" "${LIBS[@]}"
gcc "${FLAGS[@]}" -o build/test_pke.exe tests/test_pke.c "${SOURCES[@]}" "${LIBS[@]}"
gcc "${FLAGS[@]}" -o build/benchmark.exe tests/benchmark.c "${SOURCES[@]}" "${LIBS[@]}"
echo "Build finished. Run: ./build/test_pke.exe and ./build/benchmark.exe"
