
#include "player/sink.h"

Sink::Sink(sink_type_t type, EventListener *listener)
    : BufferConsumer(type == AUDIO_SINK ? "audio_sink" : "video_sink") {
  type_ = type;
  listener_ = listener;
  eos_ = false;
}

void Sink::set_eos() {
  eos_ = true;
  consume_abort(type_ == AUDIO_SINK ? AUDIO_FIFO : VIDEO_FIFO);
}

Sink::~Sink(){};
