#ifndef __FIFO_CONTROLLER_H__
#define __FIFO_CONTROLLER_H__

#include <pthread.h>
#include "third_party/FIFO/FIFO.h"

class FreeHandler {
  public:
   virtual int free_data(void *data) { return 0; };
};
class FifoProducer;
class FifoConsumer {
 public:
  FifoConsumer(fifo_t fifo, pthread_mutex_t *mutex, FreeHandler *handler);
  ~FifoConsumer();
  void set_producer(FifoProducer *producer);
  bool consume(void *data);
  int discard();
  int wait();
  int wakeup();
 private:

  pthread_mutex_t mutex_;    
  pthread_mutex_t *mutex_fifo_;    
  pthread_cond_t cond_;
  fifo_t fifo_;
  FifoProducer *producer_;
  FreeHandler *free_hdl_;
};

class FifoProducer {
 public:
  FifoProducer(fifo_t fifo, pthread_mutex_t *mutex);
  ~FifoProducer();
  void set_consumer(FifoConsumer *consumer);
  bool append(void *data);
  int wait();
  int wakeup();
 private:
  pthread_mutex_t *mutex_fifo_;    
  pthread_mutex_t mutex_;    
  pthread_cond_t cond_;    
  fifo_t fifo_;
  FifoConsumer *consumer_;
};


class FifoController {
 public:
  FifoController(FreeHandler *handler);
  ~FifoController();
  FifoConsumer *consumer_;
  FifoProducer *producer_;
 private:
  fifo_t fifo_;
  pthread_mutex_t mutex_fifo_;    
    
};

#endif
