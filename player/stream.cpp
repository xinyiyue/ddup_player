
#include <functional>

#include "log/ddup_log.h"
#include "player/demux.h"

#define TAG "Stream"

Stream::Stream(stream_type_t type)
    : BufferConsumer(type == AUDIO_STREAM ? (char *)"audio_stream"
                                          : (char *)"video_Stream") {
  eos_ = false;
  stream_type_ = type;
  dec_thread_exit_ = false;
  processer_ = nullptr;
};

Stream::~Stream(){};

int Stream::stream_on() {
  create_decoder();
  decoder_->open();
  process_thread_ = std::thread(
      std::bind(&Stream::process_thread, this, std::placeholders::_1),
      (void *)this);

  return 0;
}

int Stream::stream_off() {
  dec_thread_exit_ = true;
  consume_abort(stream_type_ == AUDIO_STREAM ? AUDIO_FIFO : VIDEO_FIFO);
  if (process_thread_.joinable()) process_thread_.join();
  decoder_->close();
  return 0;
}

int Stream::play(float speed) { return 0; }

int Stream::pause() { return 0; }

void Stream::set_eos() { eos_ = true; }

int Stream::process_raw_data(void *data, void *handle) {
  Stream *stream = (Stream *)handle;
  if (stream->processer_) stream->processer_->process_data(data);
  return 0;
}

void *Stream::process_thread(void *arg) {
  int ret = 0;
  void *data = NULL;
  void *out = NULL;
  Stream *stream = (Stream *)arg;
  while (!stream->dec_thread_exit_) {
    stream->consume(
        &data, stream->stream_type_ == AUDIO_STREAM ? AUDIO_FIFO : VIDEO_FIFO);
    ret = stream->decoder_->decode(data, &Stream::process_raw_data, this);
    if (ret < 0 && ret != DECODE_ERROR_EOS && ret != DECODE_ERROR_EAGAIN) {
      LOGE(TAG, "Decode error %d,", ret);
    }
  }
  LOGE(TAG, "%s process thread exit!!!",
       stream->stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
  return nullptr;
}
