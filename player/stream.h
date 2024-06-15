#ifndef __STREAM__H__
#define __STREAM__H__

#include "third_party/FIFO/FIFO.h"

typedef enum STREAM_TYPE { AUDIO_STREAM, VIDEO_STREAM } stream_type_t;

typedef struct av_data {
  stream_type_t type_;
  void *data;
} av_data_s;

class Stream {
 public:
  Stream(stream_type_t type) {
    eos_ = false;
    fifo_ = fifo_create(10, sizeof(void *));
    stream_type_ = type;
  };

  virtual ~Stream() { fifo_destory(fifo_); };
  virtual int open() = 0;
  virtual int play(float speed) = 0;
  virtual int pause() = 0;
  virtual int seek(long long seek_time) = 0;
  virtual int stop() = 0;
  virtual int close() = 0;
  bool append_data(void *data) { return fifo_add(fifo_, data); };
  bool is_fifo_full() { return fifo_is_full(fifo_); };
  void set_eos() { eos_ = true; }

  stream_type_t stream_type_;

 private:
  fifo_t fifo_;
  bool eos_;
};

#endif
