#ifndef _SDL_VIDEO_SINK_H__
#define _SDL_VIDEO_SINK_H__

#include <thread>

#include "gui_widget/base/texture_builder.h"
#include "player/event_listener.h"
#include "player/fifo_controller.h"
#include "player/sink.h"

class SdlVideoSink : public Sink {
 public:
  SdlVideoSink(sink_type_t type, EventListener *listener);
  virtual ~SdlVideoSink();
  virtual int init() override;
  virtual int uninit() override;
  virtual int get_supported_format(video_format_s *format) override;
  virtual int set_negotiated_format(video_format_s *format) override;

 private:
  void *video_render_thread(void *arg);
  TextureBuilder *texture_builder_;
  bool exit_;
  std::thread render_thread_id_;
};

#endif
