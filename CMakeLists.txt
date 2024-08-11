cmake_minimum_required(VERSION 3.0)
project(sdl_player)

set(KISS_SDL_SOURCE
./third_party/kiss_sdl/kiss_widgets.c
./third_party/kiss_sdl/kiss_draw.c
./third_party/kiss_sdl/kiss_general.c
./third_party/kiss_sdl/kiss_posix.c
)

set(FIFO_SOURCE
./third_party/FIFO/FIFO.c
)

set(SDL_IMPL_SOURCE
./gui_widget/sdl_impl/sdl_button.cpp
./gui_widget/sdl_impl/sdl_window.cpp
./gui_widget/sdl_impl/sdl_video.cpp
./gui_widget/sdl_impl/sdl_progress_bar.cpp
./gui_widget/sdl_impl/sdl_texture_builder.cpp
)

set(PLAYER_SOURCE
./player/ddup_player.cpp
./player/pipeline.cpp
./player/demux.cpp
./player/stream.cpp
./player/processer.cpp
./player/sink.cpp
./player/component.cpp
./player/fifo_controller.cpp
./player/ffmpeg_impl/ffmpeg_demux.cpp
./player/ffmpeg_impl/ffmpeg_stream.cpp
./player/ffmpeg_impl/ffmpeg_decoder.cpp
./player/ffmpeg_impl/ffmpeg_processer.cpp
./player/sdl_impl/sdl_video_sink.cpp
./player/sdl_impl/sdl_audio_sink.cpp
)

set(GMOCK_PLAYER_SOURCE
./player/gmock/gmock_main.cpp
./player/gmock/gmock_fifo.cpp
./player/fifo_controller.cpp
./player/gmock/gmock_sdl_audio.cpp
)

add_executable(pcw player_control_bar.cpp ${KISS_SDL_SOURCE})
add_executable(test test.cpp ${SDL_IMPL_SOURCE} ${FIFO_SOURCE} ${KISS_SDL_SOURCE} ${PLAYER_SOURCE})
add_executable(ddup_app ./app/ddup_app.cpp ${SDL_IMPL_SOURCE} ${FIFO_SOURCE} ${KISS_SDL_SOURCE} ${PLAYER_SOURCE})
add_executable(gtest ${GMOCK_PLAYER_SOURCE} ${FIFO_SOURCE})
message("当前源代码目录：" ${CMAKE_CURRENT_SOURCE_DIR})
message("当前二进制目录：" ${CMAKE_CURRENT_BINARY_DIR})
include_directories(
	/usr/include/SDL2
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg-7.0.1/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/GoogleTest/include
)

set(RESOURCE
./third_party/kiss_sdl/kiss_active.png
./third_party/kiss_sdl/kiss_bar_org.png
./third_party/kiss_sdl/kiss_bar.png
./third_party/kiss_sdl/kiss_combo.png
./third_party/kiss_sdl/kiss_down.png
./third_party/kiss_sdl/kiss_hslider.png
./third_party/kiss_sdl/kiss_left.png
./third_party/kiss_sdl/kiss_normal.png
./third_party/kiss_sdl/kiss_prelight.png
./third_party/kiss_sdl/kiss_right.png
./third_party/kiss_sdl/kiss_selected.png
./third_party/kiss_sdl/kiss_unselected.png
./third_party/kiss_sdl/kiss_up.png
./third_party/kiss_sdl/kiss_vslider.png
./third_party/kiss_sdl/kiss_wood.png
./third_party/kiss_sdl/pause.png
./third_party/kiss_sdl/picture1.png
./third_party/kiss_sdl/picture2.png
./third_party/kiss_sdl/picture3.png
./third_party/kiss_sdl/resume.png
./third_party/kiss_sdl/reload.png
./third_party/kiss_sdl/kiss_font.ttf
)
set(FFMPEG_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg-7.0.1/lib)
find_library(SDL2_LIBRARY       SDL2       /usr/lib/x86_64-linux-gnu)
find_library(SDL2_IMAGE_LIBRARY SDL2_image /usr/lib/x86_64-linux-gnu)
find_library(SDL2_TTF_LIBRARY   SDL2_ttf   /usr/lib/x86_64-linux-gnu)
find_library(PTHREAD_LIBRARY    pthread    /usr/lib/x86_64-linux-gnu)
find_library(AVFORMAT_LIBRARY   avformat   ${FFMPEG_LIB_DIR} NO_DEFAULT_PATH)
find_library(AVCODEC_LIBRARY    avcodec    ${FFMPEG_LIB_DIR} NO_DEFAULT_PATH)
find_library(AVUTIL_LIBRARY     avutil     ${FFMPEG_LIB_DIR} NO_DEFAULT_PATH)
find_library(SWSCALE_LIBRARY    swscale    ${FFMPEG_LIB_DIR} NO_DEFAULT_PATH)
find_library(SWRESAMPLE_LIBRARY swresample ${FFMPEG_LIB_DIR} NO_DEFAULT_PATH)
find_library(GTEST_LIBRARY      gtest      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/GoogleTest/lib)
find_library(GTEST_MAIN_LIBRARY gtest_main ${CMAKE_CURRENT_SOURCE_DIR}/third_party/GoogleTest/lib)
find_library(GMOCK_LIBRARY      gmock      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/GoogleTest/lib)
find_library(GMOCK_MAIN_LIBRARY gmock_main ${CMAKE_CURRENT_SOURCE_DIR}/third_party/GoogleTest/lib)
target_link_libraries(pcw ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_LIBRARY})
target_link_libraries(test ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY}  ${SDL2_LIBRARY} ${PTHREAD_LIBRARY} ${AVFORMAT_LIBRARY} ${AVCODEC_LIBRARY} ${AVUTIL_LIBRARY} ${SWSCALE_LIBRARY} ${SWRESAMPLE_LIBRARY})
target_link_libraries(ddup_app ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY}  ${SDL2_LIBRARY} ${PTHREAD_LIBRARY} ${AVFORMAT_LIBRARY} ${AVCODEC_LIBRARY} ${AVUTIL_LIBRARY} ${SWSCALE_LIBRARY} ${SWRESAMPLE_LIBRARY})
target_link_libraries(gtest ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} ${GMOCK_LIBRARY} ${GMOCK_MAIN_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY}  ${SDL2_LIBRARY} ${PTHREAD_LIBRARY} ${AVFORMAT_LIBRARY} ${AVCODEC_LIBRARY} ${AVUTIL_LIBRARY})
install(TARGETS pcw DESTINATION result)
install(TARGETS test DESTINATION result)
install(TARGETS ddup_app DESTINATION result)
install(TARGETS gtest DESTINATION result)
install(FILES ${RESOURCE} DESTINATION result)
