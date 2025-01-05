cmake_minimum_required(VERSION 3.0)
project(sdl_player)

set(FIFO_SOURCE
./third_party/FIFO/FIFO.c
)

set(SDL_IMPL_SOURCE
./gui_widget/sdl_impl/sdl_button.cpp
./gui_widget/sdl_impl/sdl_window.cpp
./gui_widget/sdl_impl/sdl_video.cpp
./gui_widget/sdl_impl/sdl_playback_panel.cpp
./gui_widget/sdl_impl/sdl_progress_bar.cpp
./gui_widget/sdl_impl/sdl_rect.cpp
./gui_widget/sdl_impl/sdl_image.cpp
./gui_widget/sdl_impl/sdl_font.cpp
./gui_widget/sdl_impl/sdl_util.cpp
./gui_widget/sdl_impl/sdl_image_widget.cpp
./gui_widget/sdl_impl/sdl_label.cpp
./gui_widget/sdl_impl/sdl_label_widget.cpp
./gui_widget/sdl_impl/sdl_texture_builder.cpp
./gui_widget/sdl_impl/sdl_vscroll_bar.cpp
./gui_widget/sdl_impl/sdl_text_box.cpp
./gui_widget/sdl_impl/sdl_combo_box.cpp
./gui_widget/sdl_impl/sdl_dirent_window.cpp
./gui_widget/sdl_impl/sdl_dirent_window_widget.cpp
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

set(GMOCK_GUI_SOURCE
./gui_widget/sdl_impl/gmock/sdl_gui_test_main.cpp
./gui_widget/sdl_impl/sdl_util.cpp
#./gui_widget/sdl_impl/gmock/sdl_image_test.cpp
#./gui_widget/sdl_impl/sdl_image.cpp
#./gui_widget/sdl_impl/gmock/sdl_font_test.cpp
#./gui_widget/sdl_impl/sdl_font.cpp
#./gui_widget/sdl_impl/gmock/sdl_rect_test.cpp
#./gui_widget/sdl_impl/sdl_rect.cpp
#./gui_widget/sdl_impl/gmock/sdl_progress_bar_test.cpp
#./gui_widget/sdl_impl/sdl_progress_bar.cpp
#./gui_widget/sdl_impl/gmock/sdl_label_test.cpp
#./gui_widget/sdl_impl/sdl_label.cpp
#./gui_widget/sdl_impl/gmock/sdl_vscroll_test.cpp
#./gui_widget/sdl_impl/sdl_vscroll_bar.cpp
#./gui_widget/sdl_impl/sdl_text_box.cpp
#./gui_widget/sdl_impl/gmock/sdl_text_box_test.cpp
#./gui_widget/sdl_impl/sdl_combo_box.cpp
#./gui_widget/sdl_impl/gmock/sdl_combo_box_test.cpp
#./gui_widget/sdl_impl/sdl_dirent_window.cpp
#./gui_widget/sdl_impl/gmock/sdl_dirent_window_test.cpp
./gui_widget/sdl_impl/sdl_texture_builder.cpp
./gui_widget/sdl_impl/sdl_gif.cpp
./gui_widget/sdl_impl/gmock/sdl_gif_test.cpp
)
add_executable(test test.cpp ${SDL_IMPL_SOURCE} ${FIFO_SOURCE} ${PLAYER_SOURCE})
add_executable(ddup_app ./app/ddup_app.cpp ${SDL_IMPL_SOURCE} ${FIFO_SOURCE} ${PLAYER_SOURCE})
add_executable(gtest ${GMOCK_PLAYER_SOURCE} ${FIFO_SOURCE})
add_executable(gtest_gui ${GMOCK_GUI_SOURCE})
message("当前源代码目录：" ${CMAKE_CURRENT_SOURCE_DIR})
message("当前二进制目录：" ${CMAKE_CURRENT_BINARY_DIR})
include_directories(
	/usr/include/SDL2
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg-7.0.1/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/GoogleTest/include
)

set(RESOURCE
./res/pause.png
./res/resume.png
./res/reload.png
./res/progress_bar.png
./res/ddup_slider.png
./res/ddup_up.png
./res/ddup_down.png
./res/ddup_player.png
./res/audio.png
./res/video.png
./res/picture.png
./res/unkown.png
./res/dir.png
./res/win_rect.png
./res/win_up.png
./res/win_down.png
./res/win_red.png
./res/enter.png
./res/win_close.png
./res/ddup_font.ttf
./res/music.gif
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
target_link_libraries(test ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY}  ${SDL2_LIBRARY} ${PTHREAD_LIBRARY} ${AVFORMAT_LIBRARY} ${AVCODEC_LIBRARY} ${AVUTIL_LIBRARY} ${SWSCALE_LIBRARY} ${SWRESAMPLE_LIBRARY})
target_link_libraries(ddup_app ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY}  ${SDL2_LIBRARY} ${PTHREAD_LIBRARY} ${AVFORMAT_LIBRARY} ${AVCODEC_LIBRARY} ${AVUTIL_LIBRARY} ${SWSCALE_LIBRARY} ${SWRESAMPLE_LIBRARY})
target_link_libraries(gtest ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} ${GMOCK_LIBRARY} ${GMOCK_MAIN_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY}  ${SDL2_LIBRARY} ${PTHREAD_LIBRARY} ${AVFORMAT_LIBRARY} ${AVCODEC_LIBRARY} ${AVUTIL_LIBRARY})
target_link_libraries(gtest_gui ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} ${GMOCK_LIBRARY} ${GMOCK_MAIN_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY}  ${SDL2_LIBRARY} ${PTHREAD_LIBRARY} ${AVFORMAT_LIBRARY} ${AVCODEC_LIBRARY} ${AVUTIL_LIBRARY})
install(TARGETS test DESTINATION result)
install(TARGETS ddup_app DESTINATION result)
install(TARGETS gtest DESTINATION result)
install(TARGETS gtest_gui DESTINATION result)
install(FILES ${RESOURCE} DESTINATION result)
