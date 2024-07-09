#!/bin/bash
TOP_DIR=$PWD

directories=("player" "player/ffmpeg_impl" "player/gmock")

for dir in "${directories[@]}"; do
    find "$dir" -name "*.cpp" -exec clang-format -style="{BasedOnStyle: Google, PointerAlignment: Right}" -i {} \;
    find "$dir" -name "*.h" -exec clang-format -style="{BasedOnStyle: Google, PointerAlignment: Right}" -i {} \;
done
