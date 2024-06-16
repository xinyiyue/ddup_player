#ifndef __STREAM__H__
#define __STREAM__H__

#include "third_party/FIFO/FIFO.h"

typedef enum STREAM_TYPE { AUDIO_STREAM, VIDEO_STREAM } stream_type_t;

typedef struct av_data {
  stream_type_t type_;
  void *data;
} av_data_s;

class Demux;
class Stream {
 public:
  Stream(stream_type_t type);
  virtual ~Stream();
  virtual int stream_on() = 0;
  virtual int stream_off() = 0;
  virtual int play(float speed) = 0;
  virtual int pause() = 0;
  virtual int clear_data(Demux *demux);
  bool append_data(void *data);
  bool is_fifo_full();
  void set_eos();

  stream_type_t stream_type_;

 private:
  fifo_t fifo_;
  bool eos_;
};

#endif
