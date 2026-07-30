#!/bin/bash
# STM32CubeMX post-generation script
# 1. Renames main.c back to main.cpp
# 2. Syncs HAL driver sources from the generated stm32cubemx CMakeLists.txt
#    into the project's <project_name>.cmake (using the submodule path)
# 3. Syncs peripheral init sources (Core/Src/*.c) into the project cmake
# 4. Cleans up generated files we don't use
#
# This script lives in scripts/ and is symlinked into each board project
# directory (boards/<board>/cubemx_post_generation.sh). It resolves paths
# relative to the symlink location, so it operates on the correct board.
#
# Configure this script in CubeMX: Project Manager → Project → Post-Generation Script

set -eo pipefail

# Resolve project directory from script location
PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
echo "project directory POST: PROJECT_DIR=$PROJECT_DIR"

PROJECT_NAME=$(basename "$PROJECT_DIR")
PROJECT_CMAKE="$PROJECT_DIR/cmake/gcc-arm-none-eabi.cmake"
GENERATED_CMAKE="$PROJECT_DIR/cmake/stm32cubemx/CMakeLists.txt"

# ── 1. Rename main.c back to main.cpp ──────────────────────────────────────
MAIN_C="$PROJECT_DIR/Core/Src/main.c"
MAIN_CPP="$PROJECT_DIR/Core/Src/main.cpp"

if [ -f "$MAIN_C" ]; then
    mv "$MAIN_C" "$MAIN_CPP"
    echo "Renamed main.c -> main.cpp"
elif [ -f "$MAIN_CPP" ]; then
    echo "main.cpp already exists, nothing to rename."
else
    echo "Warning: Neither main.c nor main.cpp found in $PROJECT_DIR/Core/Src/"
fi

if [ -f "$GENERATED_CMAKE" ]; then
    sed -i "" 's/main.c/main.cpp/' "$GENERATED_CMAKE"
    echo "Patched $CMAKE_FILE to use main.cpp"
fi

# # ── 2. Sync HAL driver sources ─────────────────────────────────────────────
# if [ ! -f "$GENERATED_CMAKE" ]; then
#     echo "Warning: Generated CMakeLists.txt not found at $GENERATED_CMAKE"
#     echo "Skipping HAL driver sync."
# else
#     if [ ! -f "$PROJECT_CMAKE" ]; then
#         echo "Error: Project cmake file not found at $PROJECT_CMAKE"
#         exit 1
#     fi

#     # Extract HAL/LL driver filenames from the generated CMakeLists.txt
#     # These are lines in the STM32_Drivers_Src block referencing STM32H5xx_HAL_Driver/Src/
#     # Strip \r in case CubeMX generates Windows line endings
#     NEW_DRIVERS=$(sed -n 's|.*STM32H5xx_HAL_Driver/Src/\([^ "]*\).*|\1|p' "$GENERATED_CMAKE" | tr -d '\r' | sort -u)

#     if [ -z "$NEW_DRIVERS" ]; then
#         echo "Warning: No HAL driver sources found in generated CMakeLists.txt"
#     else
#         echo "HAL drivers from CubeMX generation:"
#         echo "$NEW_DRIVERS" | sed 's/^/  /'

#         # Build the replacement block with submodule paths in a temp file
#         TMPBLOCK=$(mktemp)
#         while IFS= read -r driver; do
#             printf '    "${CMAKE_SOURCE_DIR}/../../STM32CubeH5/Drivers/STM32H5xx_HAL_Driver/Src/%s"\n' "$driver" >> "$TMPBLOCK"
#         done <<< "$NEW_DRIVERS"

#         # Replace the contiguous HAL driver block in the project cmake file
#         awk -v blockfile="$TMPBLOCK" '
#         /STM32CubeH5\/Drivers\/STM32H5xx_HAL_Driver\/Src\// {
#             if (!printed) {
#                 while ((getline line < blockfile) > 0) print line
#                 close(blockfile)
#                 printed = 1
#             }
#             next
#         }
#         { print }
#         ' "$PROJECT_CMAKE" > "${PROJECT_CMAKE}.tmp"

#         mv "${PROJECT_CMAKE}.tmp" "$PROJECT_CMAKE"
#         rm "$TMPBLOCK"
#         echo "Updated HAL drivers in $PROJECT_CMAKE"
#     fi
# fi

# # ── 3. Sync peripheral init sources ────────────────────────────────────────
# # CubeMX generates peripheral init files (e.g. adc.c, fdcan.c, gpio.c, tim.c)
# # in Core/Src/. We sync these into the project cmake, excluding standard files
# # that are always present (main.cpp, stm32h5xx_*, syscalls.c, sysmem.c, system_stm32h5xx.c).

# # Files that are always listed separately or not peripheral inits
# EXCLUDED_PATTERN="^(main\.(c|cpp)|stm32h5xx_.*|syscalls\.c|sysmem\.c|system_stm32h5xx\.c)$"

# PERIPH_SRCS=$(find "$PROJECT_DIR/Core/Src" -maxdepth 1 -name '*.c' -exec basename {} \; \
#     | grep -Ev "$EXCLUDED_PATTERN" \
#     | sort -u)

# if [ -z "$PERIPH_SRCS" ]; then
#     echo "No peripheral init sources found in Core/Src/"
# else
#     echo "Peripheral init sources from CubeMX generation:"
#     echo "$PERIPH_SRCS" | sed 's/^/  /'

#     # Build the replacement block
#     TMPBLOCK=$(mktemp)
#     while IFS= read -r src; do
#         printf '    "Core/Src/%s"\n' "$src" >> "$TMPBLOCK"
#     done <<< "$PERIPH_SRCS"

#     # Replace or insert the peripheral init block in the project cmake file.
#     # If peripheral lines already exist, they are replaced. If none exist,
#     # the block is inserted after the last standard Core/Src/ line
#     # (e.g. system_stm32h5xx.c).
#     awk -v blockfile="$TMPBLOCK" '
#     /["]Core\/Src\/[^"]*\.c["]/ {
#         fname = $0
#         sub(/.*Core\/Src\//, "", fname)
#         sub(/".*/, "", fname)
#         if (fname ~ /^(main\.(c|cpp)|stm32h5xx_.*|syscalls\.c|sysmem\.c|system_stm32h5xx\.c)$/) {
#             # Standard file — print it and remember this line number
#             print
#             last_std_line = NR
#             next
#         }
#         # Peripheral init line — replace the whole block on first hit
#         if (!printed) {
#             while ((getline line < blockfile) > 0) print line
#             close(blockfile)
#             printed = 1
#         }
#         next
#     }
#     {
#         # If we just moved past the last standard Core/Src line and never
#         # printed peripheral lines yet, insert them now
#         if (!printed && last_std_line > 0 && NR == last_std_line + 1) {
#             while ((getline line < blockfile) > 0) print line
#             close(blockfile)
#             printed = 1
#         }
#         print
#     }
#     ' "$PROJECT_CMAKE" > "${PROJECT_CMAKE}.tmp"

#     mv "${PROJECT_CMAKE}.tmp" "$PROJECT_CMAKE"
#     rm "$TMPBLOCK"
#     echo "Updated peripheral init sources in $PROJECT_CMAKE"
# fi

# # ── 4. Clean up generated files ────────────────────────────────────────────
# # Delete cmake/stm32cubemx/ directory
# if [ -d "$PROJECT_DIR/cmake/stm32cubemx" ]; then
#     rm -rf "$PROJECT_DIR/cmake/stm32cubemx"
#     echo "Deleted cmake/stm32cubemx/"
# fi

# # Delete starm-clang.cmake
# if [ -f "$PROJECT_DIR/cmake/starm-clang.cmake" ]; then
#     rm "$PROJECT_DIR/cmake/starm-clang.cmake"
#     echo "Deleted cmake/starm-clang.cmake"
# fi

# # Delete generated linker files (STM32H523xx_*)
# DELETED_LINKERS=0
# for f in "$PROJECT_DIR"/STM32H523xx_*; do
#     if [ -f "$f" ]; then
#         rm "$f"
#         echo "Deleted $(basename "$f")"
#         DELETED_LINKERS=$((DELETED_LINKERS + 1))
#     fi
# done
# if [ "$DELETED_LINKERS" -eq 0 ]; then
#     echo "No STM32H523xx_* linker files to delete."
# fi

# # Delete generated startup file (we use the one from the CMSIS submodule)
# if [ -f "$PROJECT_DIR/startup_stm32h523xx.s" ]; then
#     rm "$PROJECT_DIR/startup_stm32h523xx.s"
#     echo "Deleted startup_stm32h523xx.s (using submodule copy)"
# fi

# # Delete generated Drivers/ directory (we use the STM32CubeH5 submodule)
# if [ -d "$PROJECT_DIR/Drivers" ]; then
#     rm -rf "$PROJECT_DIR/Drivers"
#     echo "Deleted Drivers/"
# fi

echo "Post-generation cleanup complete."