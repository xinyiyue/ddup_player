
#include "player/sink.h"

Sink::Sink(sink_type_t type, EventListener *listener)
    : BufferConsumer(type == AUDIO_SINK ? "audio_sink" : "video_sink") {
  type_ = type;
  listener_ = listener;
}

Sink::~Sink(){};
