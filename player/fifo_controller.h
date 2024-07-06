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

#define TAGF "FIFO_CTLER"
#define MAX_EVENTS 10

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
  Fifo(int count, int size, fifo_type_t type, FreeHandler *handler);
  ~Fifo();
  int wakeup(bool flag);
  bool append(void *data);
  bool consume(void *data);
  int discard();
  int discard(void *data);
  bool is_full();
  bool is_empty();

 public:
  fifo_type_t type_;
  int efd_write_;
  int efd_read_;

 private:
  fifo_t fifo_;
  FreeHandler *free_hdl_;
  pthread_mutex_t mutex_;
};

class BufferBase {
 public:
  BufferBase();
  ~BufferBase();

  virtual bool append(void *data, fifo_type_t type = COMMON_FIFO);
  virtual bool consume(void *data, fifo_type_t type = COMMON_FIFO);
  virtual int discard(fifo_type_t type = COMMON_FIFO);

  bool bind(Fifo *fifo, bool flag);
  int wait();
  int abort(int flag, fifo_type_t type = COMMON_FIFO);
  bool is_fifo_full(fifo_type_t type = COMMON_FIFO);
  bool is_fifo_empty(fifo_type_t type = COMMON_FIFO);

  const char *get_fifo_name(fifo_type_t type);

 private:
  Fifo *get_fifo(fifo_type_t type);

 private:
  std::vector<Fifo *> fifo_arr_;
  int epollfd_;
  struct epoll_event events[MAX_EVENTS];
  int abort_flag_;
};

class BufferConsumer : public BufferBase {
 public:
  BufferConsumer(char *name) : BufferBase() { name_ = name; }
  ~BufferConsumer(){};

  bool bind_fifo(Fifo *fifo);
  bool consume_abort(fifo_type_t type = COMMON_FIFO);
  bool consume(void *data, fifo_type_t type = COMMON_FIFO) override;
  int discard(fifo_type_t type = COMMON_FIFO) override;

 private:
  std::string name_;
};

class BufferProducer : public BufferBase {
 public:
  BufferProducer(char *name) : BufferBase() { name_ = name; };
  ~BufferProducer(){};
  bool bind_fifo(Fifo *fifo);
  bool append_abort(fifo_type_t type = COMMON_FIFO);
  bool append(void *data, fifo_type_t type = COMMON_FIFO) override;

 private:
  std::string name_;
};

#endif
