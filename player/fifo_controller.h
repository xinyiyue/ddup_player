#ifndef __FIFO_CONTROLLER_H__
#define __FIFO_CONTROLLER_H__

#include <pthread.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "log/ddup_log.h"
#include "player/util.h"
#include "third_party/FIFO/FIFO.h"

#define MAX_EVENTS 10

#define DEFAULT_FIFO_SIZE 10

typedef enum FIFO_TYPE {
  COMMON_FIFO,
  AUDIO_FIFO,
  VIDEO_FIFO,
} fifo_type_t;

class FreeHandler {
 public:
  virtual int free_data(void *data) { return 0; };
};

class Fifo {
 public:
  Fifo(const char *name, int count, int size, fifo_type_t type,
       FreeHandler *handler);
  ~Fifo();
  bool append(void *data);
  bool consume(void *data);
  int discard();
  int discard(void *data);
  bool is_full();
  bool is_empty();
  const char *get_name();

 public:
  fifo_type_t type_;
  int efd_write_;
  int efd_read_;
  fifo_t fifo_;

 private:
  FreeHandler *free_hdl_;
  pthread_mutex_t mutex_;
  std::string name_;
};

class BufferBase {
 public:
  BufferBase(const char *name);
  ~BufferBase();

  bool is_fifo_full(fifo_type_t type = COMMON_FIFO);
  bool is_fifo_empty(fifo_type_t type = COMMON_FIFO);
  const char *get_fifo_name(fifo_type_t type);
  std::string name_;

 protected:
  bool append(void *data, fifo_type_t type = COMMON_FIFO);
  bool consume(void *data, fifo_type_t type = COMMON_FIFO);
  bool abort(int is_producer, fifo_type_t type = COMMON_FIFO);
  bool discard(fifo_type_t type = COMMON_FIFO);
  bool bind(Fifo *fifo, bool flag);

 private:
  Fifo *get_fifo(fifo_type_t type);
  bool wakeup(Fifo *fifo, bool is_producer);
  bool wait();

 private:
  std::vector<Fifo *> fifo_arr_;
  int epollfd_;
  struct epoll_event events[MAX_EVENTS];
  int abort_flag_;
};

class BufferConsumer : public BufferBase {
 public:
  BufferConsumer(const char *name) : BufferBase(name) {}
  ~BufferConsumer(){};

  bool bind_fifo(Fifo *fifo);
  bool consume_buffer(void *data, fifo_type_t type = COMMON_FIFO);
  bool consume_abort(fifo_type_t type = COMMON_FIFO);
  int discard_buffer(fifo_type_t type = COMMON_FIFO);
};

class BufferProducer : public BufferBase {
 public:
  BufferProducer(const char *name) : BufferBase(name){};
  ~BufferProducer(){};
  bool bind_fifo(Fifo *fifo);
  bool append_abort(fifo_type_t type = COMMON_FIFO);
  bool append_buffer(void *data, fifo_type_t type = COMMON_FIFO);
};

#endif
