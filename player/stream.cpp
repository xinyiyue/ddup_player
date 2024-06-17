
#include "log/ddup_log.h"
#include "player/demux.h"

#define TAG "Stream"

Stream::Stream(stream_type_t type) {
  eos_ = false;
  fifo_ = fifo_create(10, sizeof(void *));
  stream_type_ = type;
};

Stream::~Stream() { fifo_destory(fifo_); };

int Stream::stream_on() {
  return 0;
}

int Stream::stream_off() {
  return 0;
}

int Stream::play(float speed) {
  return 0;
}

int Stream::pause() {
  return 0;
}

bool Stream::consume_data(void *data) { return true; }


bool Stream::append_data(void *data) { return fifo_add(fifo_, data); };

bool Stream::is_fifo_full() { return fifo_is_full(fifo_); };

void Stream::set_eos() { eos_ = true; }

int Stream::flush_data(Demux *demux) {
  LOGI(TAG, "enter clear data, fifo empty:%d", fifo_is_empty(fifo_));
  void *data;
  while (!fifo_is_empty(fifo_)) {
    fifo_get(fifo_, &data);
    LOGI(TAG, "free input data:%p", data);
    demux->free_input_data(data);
  }
  return 0;
};
