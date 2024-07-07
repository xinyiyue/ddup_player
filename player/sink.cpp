
#include "player/sink.h"

Sink::Sink(sink_type_t type)
    : BufferConsumer(type == AUDIO_SINK ? "audio_sink" : "video_sink") {
  type_ = type;
}

Sink::~Sink(){};
