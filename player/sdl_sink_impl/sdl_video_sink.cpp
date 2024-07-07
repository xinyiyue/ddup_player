#include "sdl_video_sink.h"

#include "log/ddup_log.h"

#define TAG "SDLVideoSink"

extern "C" {
#include "third_party/ffmpeg-7.0.1/include/libavutil/frame.h"
}

int SDLVideoSink::open(void* codec_param) { return 0; }

int SDLVideoSink::start() { return 0; }

int SDLVideoSink::stop() { return 0; }

int SDLVideoSink::close() { return 0; }