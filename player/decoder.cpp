#include "player/decoder.h"

#include <functional>

#include "log/ddup_log.h"

#define TAG "Decoder"

int Decoder::open() {
  LOGI(TAG, "%s", "decoder open, create decoder thread");
  dec_thread_ = std::thread(
      std::bind(&Decoder::decode_thread, this, std::placeholders::_1),
      (void *)this);
  dec_thread_.detach();
  return 0;
}

int Decoder::close() {
  running = false;
  return 0;
}

/**
 * @brief read data from stream to decode.
 */
void *Decoder::decode_thread(void *arg) {
  int ret = 0;
  void *data = NULL;
  Decoder *decoder = (Decoder *)arg;
  while (decoder->running) {
    if (decoder->stream_ && decoder->stream_->consume_data(&data)) {
      ret = decoder->decode(data);
      if (ret < 0) {
        LOGE(TAG, "Decode error %d,", ret);
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }

  LOGI(TAG, "%s", "decoder thread exit.");
  return NULL;
}