#ifndef __SINK_H__
#define __SINK_H__

#include "player/buffer.h"
#include "player/event_listener.h"
#include "player/fifo_controller.h"

typedef enum SINK_TYPE { AUDIO_SINK, VIDEO_SINK } sink_type_t;

class Sink : public BufferConsumer {
 public:
  Sink(sink_type_t type, EventListener *listener);
  virtual ~Sink();
  virtual int init() { return 0; };
  virtual int uninit() { return 0; };
  virtual int get_supported_format(video_format_s *format) { return 0; };
  virtual int set_negotiated_format(video_format_s *format) { return 0; };
  virtual int get_supported_format(audio_format_s *format) { return 0; };
  virtual int set_negotiated_format(audio_format_s *format) { return 0; };

 private:
  sink_type_t type_;

 protected:
  EventListener *listener_;
};

#endif
