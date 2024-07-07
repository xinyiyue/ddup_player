#ifndef __SDL_AUDIO_SINK__H__
#define __SDL_AUDIO_SINK__H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "player/sink.h"

class SDLAudioSink : public Sink {
 public:
  SDLAudioSink() : Sink(AUDIO_SINK){};
  virtual ~SDLAudioSink(){};

  virtual int open(void* codec_param) final;
  virtual int start() final;
  virtual int stop() final;
  virtual int close() final;
  static void sdl_audio_callback_static(void* userdata, Uint8* stream,
                                        int len) {
    SDLAudioSink* instance = static_cast<SDLAudioSink*>(userdata);
    instance->sdl_audio_callback(instance, stream, len);
  }

 private:
  SDL_AudioDeviceID audio_dev;
  int get_audio_frame_info(void* frame);
  int open_sdl_device(void* channel_layout, int wanted_sample_rate);

 private:
  void sdl_audio_callback(void* opaque, Uint8* stream, int len);
  const int sdl_audio_min_buffer_size = 512;
  const int sdl_max_cb_per_sec = 30;
};

#endif
