#include "player/sdl_impl/sdl_audio_sink.h"

#include <functional>

#include "log/ddup_log.h"

#define TAG "SdlAudioSink"

SdlAudioSink::SdlAudioSink(sink_type_t type, EventListener *listener)
    : Sink(type, listener) {
  exit_ = false;
}

SdlAudioSink::~SdlAudioSink() {}

int SdlAudioSink::init() {
  render_thread_id_ = std::thread(std::bind(&SdlAudioSink::audio_render_thread,
                                            this, std::placeholders::_1),
                                  (void *)this);
  return 0;
}

int SdlAudioSink::uninit() {
  exit_ = true;
  consume_abort(AUDIO_FIFO);
  return 0;
}

void *SdlAudioSink::audio_render_thread(void *arg) {
  SdlAudioSink *sink = (SdlAudioSink *)arg;
  while (!exit_) {
    render_buffer_s *buff;
    bool ret = sink->consume_buffer(&buff, AUDIO_FIFO);
    if (!ret) {
      LOGE(TAG, "%s", "consume buffer error");
      continue;
    }
    free(buff->data);
    free(buff);
    SDL_Delay(30);  // 30ms
  }
  LOGI(TAG, "%s", "audio render thread exit!!!!");
  return nullptr;
}
