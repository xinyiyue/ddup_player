#ifndef __SINK_H__
#define __SINK_H__

#include "player/fifo_controller.h"

typedef enum SINK_TYPE { AUDIO_SINK, VIDEO_SINK } sink_type_t;

class Sink : public BufferConsumer {
 public:
  Sink(sink_type_t type);
  virtual ~Sink();

 private:
  sink_type_t type_;
};

#endif
