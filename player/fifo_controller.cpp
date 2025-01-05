#include "player/fifo_controller.h"

#define TAGF "FCTLER"

Fifo::Fifo(const char *name, int count, int size, fifo_type_t type,
           FreeHandler *handler) {
  name_ = name;
  type_ = type;
  free_hdl_ = handler;
  pthread_mutex_init(&mutex_, NULL);
  efd_write_ = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  efd_read_ = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  LOGD(TAGF, "fifo type:%d,write fd:%d, read fd:%d", type_, efd_write_,
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

bool Fifo::append(void *data) {
  AutoLock lock(&mutex_);
  int ret = fifo_add(fifo_, data);
  if (!ret) {
    LOGE(TAGF, "%s add failed:%d", name_.c_str(), ret);
  }
  return ret;
}

bool Fifo::consume(void *data) {
  AutoLock lock(&mutex_);
  bool ret = fifo_get(fifo_, data);
  if (!ret) {
    LOGE(TAGF, "%s get failed:%d", name_.c_str(), ret);
  }
  return ret;
}

int Fifo::discard() {
  AutoLock lock(&mutex_);
  void *data;
  while (!fifo_is_empty(fifo_)) {
    fifo_get(fifo_, &data);
    LOGD(TAGF, "%s discard,free input data:%p", name_.c_str(), data);
    free_hdl_->free_data(data);
  }
  LOGD(TAGF, "%s discard data finish,wake up producer", name_.c_str());
  return 0;
}

int Fifo::discard(void *data) {
  AutoLock lock(&mutex_);
  LOGD(TAGF, "discard once, free input data:%p", data);
  free_hdl_->free_data(data);
  return 0;
}

bool Fifo::is_full() { return fifo_is_full(fifo_); }

bool Fifo::is_empty() { return fifo_is_empty(fifo_); }

const char *Fifo::get_name() { return name_.c_str(); }

BufferBase::BufferBase(const char *name) : name_(name) {
  abort_flag_ = 0;
  epollfd_ = epoll_create1(EPOLL_CLOEXEC);
  if (epollfd_ < 0) {
    perror("create epoll error:");
  }
  LOGD(TAGF, "create epoll fd:%d", epollfd_);
}

BufferBase::~BufferBase() {
  if (epollfd_) {
    close(epollfd_);
  }
}

const char *BufferBase::get_fifo_name(fifo_type_t type) {
  Fifo *fifo = get_fifo(type);
  if (!fifo) {
    LOGE(TAGF, "get fifo faied, type:%d", type);
    return "unkown_fifo";
  }
  return fifo->get_name();
};

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
  LOGD(TAGF, "add %s fd:%d to epoll fd:%d", flag ? "wrting" : "reading",
       ev.data.fd, epollfd_);
  return true;
}

bool BufferBase::is_fifo_full(fifo_type_t type) {
  Fifo *fifo = get_fifo(type);
  if (!fifo) {
    LOGE(TAGF, "get fifo faied, type:%d, name:%s", type, fifo->get_name());
    return false;
  }
  return fifo->is_full();
}

bool BufferBase::is_fifo_empty(fifo_type_t type) {
  Fifo *fifo = get_fifo(type);
  if (!fifo) {
    LOGE(TAGF, "get fifo faied, type:%d, name:%s", type, get_fifo_name(type));
    return false;
  }
  return fifo->is_empty();
}

bool BufferBase::append(void *data, fifo_type_t type) {
  bool ret = true;
  Fifo *fifo = get_fifo(type);
  if (!fifo) {
    LOGE(TAGF, "get fifo faied, type:%d, name:%s", type, get_fifo_name(type));
    return false;
  }
  while (fifo->is_full()) {
    LOGD(TAGF,
         "%s is full, producer:%s wait consumer to consume data,pending "
         "buffer:%p",
         fifo->get_name(), name_.c_str(), *(void **)data);
    wait();
    if (abort_flag_) {
      ret = fifo->discard(*(void **)data);
      if (!ret) {
        LOGD(TAGF, "%s is full,producer:%s discard data:%p failed",
             fifo->get_name(), name_.c_str(), *(void **)data);
      }
      abort_flag_ = 0;
      return false;
    }
  }
  abort_flag_ = 0;

  LOGD(TAGF, "producer %s append buffer:%p to fifo:%s", name_.c_str(),
       *(void **)data, fifo->get_name());
  ret = fifo->append(data);
  if (ret) {
    ret = wakeup(fifo, false);
  } else {
    LOGE(TAGF, "producer %s append buffer:%p to fifo:%s failed", name_.c_str(),
         *(void **)data, fifo->get_name());
  }

  return ret;
}

bool BufferBase::consume(void *data, fifo_type_t type) {
  bool ret = true;
  Fifo *fifo = get_fifo(type);
  if (!fifo) {
    LOGE(TAGF, "get fifo faied, type:%d, name:%s", type, get_fifo_name(type));
    return false;
  }
  while (fifo->is_empty() && !abort_flag_) {
    LOGD(TAGF, "%s is empty,consumer:%s wait producer to produce data",
         fifo->get_name(), name_.c_str());
    wait();
  }
  if (abort_flag_) {
    LOGD(TAGF, "%s is empty, and consumer:%s abort consume", fifo->get_name(),
         name_.c_str());
    abort_flag_ = false;
    ret = false;
  } else {
    ret = fifo->consume(data);
    if (ret) {
      LOGD(TAGF, "consumer %s consume buffer:%p from fifo:%s", name_.c_str(),
           *(void **)data, fifo->get_name());
      ret = wakeup(fifo, true);
    } else {
      LOGE(TAGF, "consumer %s consume buffer from fifo:%s failed",
           name_.c_str(), fifo->get_name());
    }
  }
  return ret;
}

bool BufferBase::discard(fifo_type_t type) {
  bool ret = 0;
  Fifo *fifo = get_fifo(type);
  if (!fifo) {
    LOGE(TAGF, "get fifo faied, type:%d, name:%s", type, get_fifo_name(type));
    return false;
  }
  LOGE(TAGF, "consumer %s discard buffer from fifo:%s", name_.c_str(),
       fifo->get_name());
  fifo->discard();
  ret = wakeup(fifo, true);
  return ret;
}

bool BufferBase::abort(int is_producer, fifo_type_t type) {
  bool ret = true;
  Fifo *fifo = get_fifo(type);
  if (!fifo) {
    LOGE(TAGF, "ABORT,get fifo faied, type:%d, name:%s", type,
         fifo->get_name());
    return false;
  }
  abort_flag_ = 1;
  ret = wakeup(fifo, is_producer);
  return ret;
}

bool BufferBase::wakeup(Fifo *fifo, bool is_producer) {
  int event_count = 0;
  char test[10] = "123456789";
  int efd = 0;
  if (is_producer) {
    efd = fifo->efd_write_;
  } else {
    efd = fifo->efd_read_;
  }
  LOGD(TAGF, "%s write string:%s event to %s fd:%d to wakeup %s", name_.c_str(),
       test, is_producer ? "writing" : "reading", efd,
       is_producer ? "producer" : "consumer");
  write(efd, test, 10);
  return true;
}

bool BufferBase::wait() {
  LOGD(TAGF, "%s wait event from eoll fd: %d", name_.c_str(), epollfd_);
  int nfds = epoll_wait(epollfd_, events, MAX_EVENTS, -1);
  if (nfds == -1) {
    perror("epoll_wait");
    return false;
  }
  for (int i = 0; i < nfds; i++) {
    char test[10] = {'0'};
    read(events[i].data.fd, test, 10);
    LOGD(TAGF, "%s Received event: %s from fd:%d, epoll fd:%d wakeup",
         name_.c_str(), test, events[i].data.fd, epollfd_);
  }
  return true;
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

bool BufferConsumer::bind_fifo(Fifo *fifo) { return bind(fifo, false); }

bool BufferConsumer::consume_abort(fifo_type_t type) {
  return abort(false, type);
}

bool BufferConsumer::consume_buffer(void *data, fifo_type_t type) {
  return BufferBase::consume(data, type);
}

int BufferConsumer::discard_buffer(fifo_type_t type) {
  return BufferBase::discard(type);
}

bool BufferProducer::bind_fifo(Fifo *fifo) { return bind(fifo, true); }

bool BufferProducer::append_abort(fifo_type_t type) {
  return abort(true, type);
}

bool BufferProducer::append_buffer(void *data, fifo_type_t type) {
  return BufferBase::append(data, type);
}
