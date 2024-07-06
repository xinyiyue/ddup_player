#ifndef __PROCESSER__H__
#define __PROCESSER__H__

#include "player/fifo_controller.h"

class Processer : public BufferProducer, public FreeHandler {
 public:
  Processer();
  virtual ~Processer();
  virtual int init(void *format);
  virtual int process_data(void *data);
  virtual int uninit();
  virtual int set_speead(float speed);

 private:
  Fifo *raw_fifo_;
};

#endif
