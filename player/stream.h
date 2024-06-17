#ifndef __STREAM__H__
#define __STREAM__H__

#include "player/decoder.h"
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
  virtual int stream_on();
  virtual int stream_off();
  virtual int play(float speed);
  virtual int pause();
  int  flush_data(Demux *demux);
  bool append_data(void *data);
  bool consume_data(void *data);
  bool is_fifo_full();
  void set_eos();

  stream_type_t stream_type_;

 private:
  fifo_t fifo_;
  bool eos_;
  Decoder *decoder_;
};

#endif
