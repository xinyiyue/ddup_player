cmake_minimum_required(VERSION 3.0)
project(sdl_player)
add_executable(pcw player_control_bar.cpp ./kiss_sdl/kiss_widgets.c ./kiss_sdl/kiss_draw.c ./kiss_sdl/kiss_general.c ./kiss_sdl/kiss_posix.c)
add_executable(test test.cpp sdl_progress_bar.cpp sdl_button.cpp sdl_window.cpp ./kiss_sdl/kiss_widgets.c ./kiss_sdl/kiss_draw.c ./kiss_sdl/kiss_general.c ./kiss_sdl/kiss_posix.c)
include_directories(
	/usr/include/SDL2
	./kiss_sdl
)

set(RESOURCE
./kiss_sdl/kiss_active.png
./kiss_sdl/kiss_bar_org.png
./kiss_sdl/kiss_bar.png
./kiss_sdl/kiss_combo.png
./kiss_sdl/kiss_down.png
./kiss_sdl/kiss_hslider.png
./kiss_sdl/kiss_left.png
./kiss_sdl/kiss_normal.png
./kiss_sdl/kiss_prelight.png
./kiss_sdl/kiss_right.png
./kiss_sdl/kiss_selected.png
./kiss_sdl/kiss_unselected.png
./kiss_sdl/kiss_up.png
./kiss_sdl/kiss_vslider.png
./kiss_sdl/kiss_wood.png
./kiss_sdl/pause.png
./kiss_sdl/picture1.png
./kiss_sdl/picture2.png
./kiss_sdl/picture3.png
./kiss_sdl/resume.png
./kiss_sdl/kiss_font.ttf
)

find_library(SDL2_LIBRARY       SDL2       /usr/lib/x86_64-linux-gnu)
find_library(SDL2_IMAGE_LIBRARY SDL2_image /usr/lib/x86_64-linux-gnu)
find_library(SDL2_TTF_LIBRARY   SDL2_ttf   /usr/lib/x86_64-linux-gnu)
target_link_libraries(pcw ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_LIBRARY})
target_link_libraries(test ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY}  ${SDL2_LIBRARY})
install(TARGETS pcw DESTINATION result)
install(TARGETS test DESTINATION result)
install(FILES ${RESOURCE} DESTINATION result)
