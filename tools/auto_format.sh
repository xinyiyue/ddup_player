#!/bin/bash
TOP_DIR=$PWD

directories=("app" "player" "gui_widget" "log")

for dir in "${directories[@]}"; do
    echo "format dir: $dir"
    find "$dir" -name "*.cpp" -exec clang-format -style="{BasedOnStyle: Google, PointerAlignment: Right}" -i {} \;
    find "$dir" -name "*.h" -exec clang-format -style="{BasedOnStyle: Google, PointerAlignment: Right}" -i {} \;
done
