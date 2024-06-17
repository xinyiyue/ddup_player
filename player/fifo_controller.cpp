
#include "player/fifo_controller.h"
#include "player/component.h"
#include "log/ddup_log.h"

#define TAG "FifoController"

FifoConsumer::FifoConsumer(fifo_t fifo, pthread_mutex_t *mutex, FreeHandler *handler) {
  pthread_cond_init(&cond_, NULL);
  pthread_mutex_init(&mutex_, NULL);
  fifo_ = fifo;
  mutex_fifo_ = mutex;
  free_hdl_ = handler;
}

FifoConsumer::~FifoConsumer() {
  pthread_cond_destroy(&cond_);
  pthread_mutex_destroy(&mutex_);
}

bool FifoConsumer::consume(void *data) {
  AutoLock lock(mutex_fifo_);
  bool ret = fifo_get(fifo_, data);
  if (!ret) {
    LOGE(TAG, "fifo get failed:%d", ret);
    return ret;
  }
  producer_->wakeup();
  return ret;
}

void FifoConsumer::set_producer(FifoProducer *producer) {
  producer_ = producer;
}

int FifoConsumer::discard() {
  AutoLock lock(mutex_fifo_);
  void *data;
  while (!fifo_is_empty(fifo_)) {
    fifo_get(fifo_, &data);
    LOGI(TAG, "free input data:%p", data);
    free_hdl_->free_data(data);
  }
  return 0;
}

int FifoConsumer::wait() {
  pthread_mutex_lock(&mutex_);
  pthread_cond_wait(&cond_, &mutex_);
  pthread_mutex_unlock(&mutex_);
  return 0;
}

int FifoConsumer::wakeup() {
  pthread_mutex_lock(&mutex_);
  pthread_cond_signal(&cond_);
  pthread_mutex_unlock(&mutex_);
  return 0;
}

FifoProducer::FifoProducer(fifo_t fifo, pthread_mutex_t *mutex) {
  fifo_ = fifo;
  mutex_fifo_ = mutex;
}

FifoProducer::~FifoProducer() {
  pthread_cond_destroy(&cond_);
  pthread_mutex_destroy(&mutex_);
}

void FifoProducer::set_consumer(FifoConsumer *consumer) {
  consumer_ = consumer;
}

bool FifoProducer::append(void *data) {
  AutoLock lock(mutex_fifo_);
  int ret = fifo_add(fifo_, data);
  if (!ret) {
    LOGE(TAG, "fifo add failed:%d", ret);
    return ret;
  }
  consumer_->wakeup();
  return ret;
}

int FifoProducer::wait() {
  pthread_mutex_lock(&mutex_);
  pthread_cond_wait(&cond_, &mutex_);
  pthread_mutex_unlock(&mutex_);
  return 0;
}

int FifoProducer::wakeup() {
  pthread_mutex_lock(&mutex_);
  pthread_cond_signal(&cond_);
  pthread_mutex_unlock(&mutex_);
  return 0;
}


FifoController::FifoController(FreeHandler *handler) {
  pthread_mutex_init(&mutex_fifo_, NULL);
  fifo_ = fifo_create(10, sizeof(void *));
  consumer_ = new FifoConsumer(fifo_, &mutex_fifo_, handler);
  producer_ = new FifoProducer(fifo_, &mutex_fifo_);
  producer_->set_consumer(consumer_);
  consumer_->set_producer(producer_);
}

FifoController::~FifoController() {
  pthread_mutex_destroy(&mutex_fifo_);
  fifo_destory(fifo_);
  delete consumer_;
  delete producer_;
}
