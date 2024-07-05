#include "player/fifo_controller.h"

#define TAGF "FIFO_CTLER"

Fifo::Fifo(int count, int size, fifo_type_t type, FreeHandler *handler) {
  type_ = type;
  free_hdl_ = handler;
  pthread_mutex_init(&mutex_, NULL);
  efd_write_ = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  efd_read_ = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  LOGE(TAGF, "fifo type:%d,write fd:%d, read fd:%d", type_, efd_write_,
       efd_read_);
  fifo_ = fifo_create(count, size);
};

Fifo::~Fifo() {
  fifo_destory(fifo_);
  pthread_mutex_destroy(&mutex_);
  if (efd_write_) {
    close(efd_write_);
  }
  if (efd_read_) {
    close(efd_read_);
  }
};

int Fifo::wakeup(bool flag) {
  int event_count = 0;
  char test[10] = "123456789";
  int efd = 0;
  if (flag) {
    efd = efd_write_;
  } else {
    efd = efd_read_;
  }
  LOGE(TAGF, "write string:%s event to %s fd:%d", test,
       flag ? "writing" : "reading", efd);
  write(efd, test, 10);
  return 0;
}

bool Fifo::append(void *data) {
  AutoLock lock(&mutex_);
  int ret = fifo_add(fifo_, data);
  if (!ret) {
    LOGE(TAGF, "fifo add failed:%d", ret);
  }
  wakeup(false);
  return ret;
}

bool Fifo::consume(void *data) {
  AutoLock lock(&mutex_);
  bool ret = fifo_get(fifo_, data);
  if (!ret) {
    LOGE(TAGF, "fifo get failed:%d", ret);
  }
  wakeup(true);
  return ret;
}

int Fifo::discard() {
  AutoLock lock(&mutex_);
  void *data;
  while (!fifo_is_empty(fifo_)) {
    fifo_get(fifo_, &data);
    LOGI(TAGF, "free input data:%p", data);
    free_hdl_->free_data(data);
  }
  wakeup(true);
  return 0;
}

int Fifo::discard(void *data) {
  AutoLock lock(&mutex_);
  LOGI(TAGF, "free input data:%p", data);
  free_hdl_->free_data(data);
  return 0;
}

bool Fifo::is_full() { return fifo_is_full(fifo_); }

bool Fifo::is_empty() { return fifo_is_empty(fifo_); }

BufferBase::BufferBase() {
  abort_flag_ = 0;
  epollfd_ = epoll_create1(EPOLL_CLOEXEC);
  if (epollfd_ < 0) {
    perror("create epoll error:");
  }
  LOGI(TAGF, "create epoll fd:%d", epollfd_);
}

BufferBase::~BufferBase() {
  if (epollfd_) {
    close(epollfd_);
  }
}

bool BufferBase::bind(Fifo *fifo, bool flag) {
  fifo_arr_.push_back(fifo);
  struct epoll_event ev;
  ev.events = EPOLLIN;
  if (flag)
    ev.data.fd = fifo->efd_write_;
  else
    ev.data.fd = fifo->efd_read_;

  int ret = epoll_ctl(epollfd_, EPOLL_CTL_ADD, ev.data.fd, &ev);
  if (ret == -1) {
    LOGE(TAGF, "add epoll failed, reading fd:%d", fifo->efd_read_);
    return false;
  }
  LOGE(TAGF, "add %s fd:%d to epoll fd:%d", flag ? "wrting" : "reading",
       ev.data.fd, epollfd_);
  return true;
}

bool BufferBase::is_fifo_full(fifo_type_t type) {
  Fifo *fifo = get_fifo(type);
  if (!fifo) {
    LOGE(TAGF, "get fifo faied, type:%d", type);
    return false;
  }
  return fifo->is_full();
}

bool BufferBase::is_fifo_empty(fifo_type_t type) {
  Fifo *fifo = get_fifo(type);
  if (!fifo) {
    LOGE(TAGF, "get fifo faied, type:%d", type);
    return false;
  }
  return fifo->is_empty();
}

bool BufferBase::append(void *data, fifo_type_t type) {
  Fifo *fifo = get_fifo(type);
  if (!fifo) {
    LOGE(TAGF, "get fifo faied, type:%d", type);
    return false;
  }
  void *temp = nullptr;
  while (fifo->is_full() && !abort_flag_) {
    memcpy(&temp, data, sizeof(void *));
    LOGE(TAGF, "fifo is full, wait consumer to consume data,pending buffer:%p",temp);
    wait();
  }
  
  if (abort_flag_) {
    LOGE(TAGF,"%s", "fifo is full, and abort append, discard data:%p", temp);
    fifo->discard(temp);
    abort_flag_ = 0;
  } else {
    fifo->append(data);
  }
  return true;
}

bool BufferBase::consume(void *data, fifo_type_t type) {
  Fifo *fifo = get_fifo(type);
  if (!fifo) {
    LOGE(TAGF, "get fifo faied, type:%d", type);
    return false;
  }
  while (fifo->is_empty()) {
    LOGE(TAGF, "%s", "fifo is empty, wait producer to produce data");
    wait();
  }
  fifo->consume(data);
  return true;
}

int BufferBase::discard(fifo_type_t type) {
  Fifo *fifo = get_fifo(type);
  if (!fifo) {
    LOGE(TAGF, "get fifo faied, type:%d", type);
    return false;
  }
  fifo->discard();
  return 0;
}

int BufferBase::abort(int flag, fifo_type_t type) {
  Fifo *fifo = get_fifo(type);
  if (!fifo) {
    LOGE(TAGF, "get fifo faied, type:%d", type);
    return false;
  }
  abort_flag_ = 1;
  fifo->wakeup(flag);
  return 0;
}

int BufferBase::wait() {
  LOGE(TAGF, "wait eoll fd: %d\n", epollfd_);
  int nfds = epoll_wait(epollfd_, events, MAX_EVENTS, -1);
  if (nfds == -1) {
    perror("epoll_wait");
    return -1;
  }
  for (int i = 0; i < nfds; i++) {
    char test[10] = {'0'};
    read(events[i].data.fd, test, 10);
    LOGE(TAGF, "\nReceived event: %s from fd:%d, epoll fd:%d wakeup\n", test,
         events[i].data.fd, epollfd_);
  }
  return 0;
}

Fifo *BufferBase::get_fifo(fifo_type_t type) {
  Fifo *ret = nullptr;
  for (int i = 0; i < fifo_arr_.size(); i++) {
    if (fifo_arr_[i]->type_ == type) {
      ret = fifo_arr_[i];
      break;
    }
  }
  return ret;
}

bool BufferConsumer::bind_fifo(Fifo *fifo) {
  bind(fifo, false);
  return true;
}

bool BufferConsumer::consume_abort(fifo_type_t type) {
  abort(false, type);
  return true;
}

bool BufferConsumer::consume(void *data, fifo_type_t type) {
  LOGE(TAGF, "consumer:%s consume data from fifo:%d", name_.c_str(), type);
  return BufferBase::consume(data, type);
}

int BufferConsumer::discard(fifo_type_t type) {
  LOGE(TAGF, "consumer:%s discard data for fifo:%d", name_.c_str(), type);
  return BufferBase::discard(type);
}

bool BufferProducer::bind_fifo(Fifo *fifo) {
  bind(fifo, true);
  return true;
}

bool BufferProducer::append_abort(fifo_type_t type) {
  abort(true, type);
  return true;
}

bool BufferProducer::append(void *data, fifo_type_t type) {
  LOGE(TAGF, "producer:%s append data for fifo:%d", name_.c_str(), type);
  return BufferBase::append(data, type);
}
