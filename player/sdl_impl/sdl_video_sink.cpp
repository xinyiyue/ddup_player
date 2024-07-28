#include "player/sdl_impl/sdl_video_sink.h"

#include <functional>

#include "log/ddup_log.h"

#define TAG "SdlVideoSink"

SdlVideoSink::SdlVideoSink(sink_type_t type, EventListener *listener)
    : Sink(type, listener) {
  exit_ = false;
}

SdlVideoSink::~SdlVideoSink() {}

int SdlVideoSink::init() {
  if (!listener_) {
    LOGE(TAG, "%s", "listener is null, init failed");
    return -1;
  }
  EventListener *top = listener_->get_top_listener();
  if (!top) {
    LOGE(TAG, "%s", "get top listener failed");
    return -1;
  }
  texture_builder_ = dynamic_cast<TextureBuilder *>(top);
  LOGE(TAG, "get texture_builder_:%s success", texture_builder_->name_.c_str());
  render_thread_id_ = std::thread(std::bind(&SdlVideoSink::video_render_thread,
                                            this));
  return 0;
}

int SdlVideoSink::uninit() {
  LOGI(TAG, "%s", "uninit");
  exit_ = true;
  consume_abort(VIDEO_FIFO);
  if (render_thread_id_.joinable()) render_thread_id_.join();
  return 0;
}

int SdlVideoSink::get_supported_format(video_format_s *format) {
  return texture_builder_->get_supported_format(format);
}

int SdlVideoSink::set_negotiated_format(video_format_s *format) {
  return texture_builder_->set_negotiated_format(format);
}

void SdlVideoSink::video_render_thread(void) {
  while (!exit_) {
    if (eos_ && is_fifo_empty(VIDEO_FIFO)) {
      listener_->notify_event(DDUP_EVENT_VIDEO_EOS, nullptr);
      LOGI(TAG, "%s", "notify VIDEO_EOS");
      eos_ = false;
    }
    render_buffer_s *buff;
    bool ret = consume_buffer(&buff, VIDEO_FIFO);
    if (!ret) {
      LOGE(TAG, "%s", "consume buffer error");
      continue;
    }
    texture_builder_->build_texture(buff);
    int sleep_time = 1000 / buff->frame_rate - 2;
    free(buff->data);
    free(buff);
    SDL_Delay(sleep_time);  // 50ms
  }
  LOGI(TAG, "%s", "video render thread exit!!!!");
}
