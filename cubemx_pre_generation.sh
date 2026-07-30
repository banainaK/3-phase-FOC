#!/bin/bash
# STM32CubeMX pre-generation script
# Renames main.cpp to main.c so CubeMX can regenerate it.
#
# This script lives in scripts/ and is symlinked into each board project
# directory (boards/<board>/cubemx_pre_generation.sh). It resolves paths
# relative to the symlink location, so it operates on the correct board.
#
# Set it as the Pre-Generation Script in CubeMX: Project Manager → Project.

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
echo "project directory PRE: PROJECT_DIR=$PROJECT_DIR"

MAIN_CPP="$PROJECT_DIR/Core/Src/main.cpp"
MAIN_C="$PROJECT_DIR/Core/Src/main.c"

if [ -f "$MAIN_CPP" ]; then
    mv "$MAIN_CPP" "$MAIN_C"
    echo "Renamed main.cpp -> main.c"
elif [ -f "$MAIN_C" ]; then
    echo "main.c already exists, nothing to rename."
else
    echo "Warning: Neither main.cpp nor main.c found in $PROJECT_DIR/Core/Src/"
fi