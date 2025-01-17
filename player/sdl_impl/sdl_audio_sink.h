#ifndef _SDL_AUDIO_SINK_H__
#define _SDL_AUDIO_SINK_H__

#include <thread>

#include "player/buffer.h"
#include "player/event_listener.h"
#include "player/fifo_controller.h"
#include "player/sink.h"

class SdlAudioSink : public Sink {
 public:
  SdlAudioSink(sink_type_t type, EventListener *listener);
  virtual ~SdlAudioSink();
  virtual int init() override;
  virtual int uninit() override;
  virtual int get_supported_format(audio_format_s *format) override;
  virtual int set_negotiated_format(audio_format_s *format) override;

 private:
  static void audio_callback(void *userdata, Uint8 *stream, int len);
  void audio_render_thread(void);
  bool exit_;
  std::thread render_thread_id_;
  SDL_AudioDeviceID audio_dev_ = 0;
  audio_format_s aformat;
  bool is_start = false;
  render_buffer_s *last_buff_;
  int last_buff_offset_;
};

#endif
