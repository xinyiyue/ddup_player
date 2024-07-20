#ifndef __STREAM__H__
#define __STREAM__H__

#include <thread>

#include "player/decoder.h"
#include "player/fifo_controller.h"
#include "player/processer.h"
#include "third_party/FIFO/FIFO.h"

typedef enum STREAM_TYPE { AUDIO_STREAM, VIDEO_STREAM } stream_type_t;

typedef struct av_data {
  stream_type_t type_;
  void *data;
} av_data_s;

class Stream : public BufferConsumer, public EventListener {
 public:
  Stream(stream_type_t type, EventListener *listener);
  virtual ~Stream();
  virtual int create_decoder() = 0;
  virtual int create_processer() = 0;
  virtual int stream_on();
  virtual int stream_off();
  virtual int set_speed(float speed);
  static int process_raw_data(void *data, void *handle);
  int flush();
  void set_eos();
  stream_type_t stream_type_;
  bool dec_thread_exit_;
  bool need_flush_;

  Decoder *decoder_;
  Processer *processer_;

 private:
  void *process_thread(void *arg);

  bool eos_;
  std::thread process_thread_;
};

#endif
