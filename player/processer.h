#ifndef __PROCESSER__H__
#define __PROCESSER__H__

#include "player/fifo_controller.h"
#include "player/sink.h"

typedef enum PROCESSER_TYPE {
  AUDIO_PROCESSER,
  VIDEO_PROCESSER
} processer_type_t;

class Processer : public BufferProducer, public FreeHandler {
 public:
  Processer(processer_type_t type, void *codec_param);
  virtual ~Processer();
  virtual int init();
  virtual int process_data(void *data);
  virtual int process(void *data) = 0;
  virtual int uninit();
  virtual int set_speead(float speed);
  int flush();

 private:
  Fifo *raw_fifo_;
  processer_type_t type_;
  void *codec_param_;
  Sink *sink_;
};

#endif
