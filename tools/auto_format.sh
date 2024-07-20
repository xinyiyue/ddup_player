#!/bin/bash
TOP_DIR=$PWD

directories=("app" "player" "player/ffmpeg_impl" "player/sdl_impl" "player/gmock" "gui_widget/base" "gui_widget/sdl_impl")

for dir in "${directories[@]}"; do
    find "$dir" -name "*.cpp" -exec clang-format -style="{BasedOnStyle: Google, PointerAlignment: Right}" -i {} \;
    find "$dir" -name "*.h" -exec clang-format -style="{BasedOnStyle: Google, PointerAlignment: Right}" -i {} \;
done
