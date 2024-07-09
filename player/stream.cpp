
#include <functional>

#include "log/ddup_log.h"
#include "player/demux.h"

#define TAG "Stream"

Stream::Stream(stream_type_t type)
    : BufferConsumer(type == AUDIO_STREAM ? "audio_stream" : "video_Stream") {
  eos_ = false;
  stream_type_ = type;
  dec_thread_exit_ = false;
  processer_ = nullptr;
  need_flush_ = false;
};

Stream::~Stream() {
  if (decoder_) {
    delete decoder_;
    decoder_ = nullptr;
  }
  if (processer_) {
    delete processer_;
    processer_ = nullptr;
  }
};

int Stream::stream_on() {
  create_decoder();
  create_processer();
  decoder_->open();
  processer_->init();
  process_thread_ = std::thread(
      std::bind(&Stream::process_thread, this, std::placeholders::_1),
      (void *)this);

  return 0;
}

int Stream::flush() {
  LOGE(TAG, "%s stream flush",
       stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
  consume_abort(stream_type_ == AUDIO_STREAM ? AUDIO_FIFO : VIDEO_FIFO);
  processer_->append_abort(stream_type_ == AUDIO_STREAM ? AUDIO_FIFO
                                                        : VIDEO_FIFO);
  need_flush_ = true;
  return 0;
}

int Stream::stream_off() {
  LOGE(TAG, "%s stream off", stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
  dec_thread_exit_ = true;
  consume_abort(stream_type_ == AUDIO_STREAM ? AUDIO_FIFO : VIDEO_FIFO);
  processer_->append_abort(stream_type_ == AUDIO_STREAM ? AUDIO_FIFO
                                                        : VIDEO_FIFO);
  if (process_thread_.joinable()) process_thread_.join();
  LOGE(TAG, "%s stream off, processer unint",
       stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
  processer_->uninit();
  decoder_->close();
  return 0;
}

int Stream::play(float speed) { return 0; }

int Stream::pause() { return 0; }

void Stream::set_eos() { eos_ = true; }

int Stream::process_raw_data(void *data, void *handle) {
  Stream *stream = (Stream *)handle;
  if (stream->processer_) {
    LOGE(TAG, "process data %p", data);
    stream->processer_->process_data(data);
  }
  return 0;
}

void *Stream::process_thread(void *arg) {
  int ret = 0;
  void *data = NULL;
  void *out = NULL;
  Stream *stream = (Stream *)arg;
  while (!stream->dec_thread_exit_) {
    data = nullptr;
    stream->consume_buffer(
        &data, stream->stream_type_ == AUDIO_STREAM ? AUDIO_FIFO : VIDEO_FIFO);
    if (!data) {
      LOGE(TAG, "%s processer consume data empty,continue",
           stream->stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
      continue;
    }
    ret = stream->decoder_->decode(data, &Stream::process_raw_data, this);
    if (ret < 0 && ret != DECODE_ERROR_EOS && ret != DECODE_ERROR_EAGAIN) {
      LOGE(TAG, "Decode error %d,", ret);
    }
    if (stream->need_flush_) {
      LOGE(TAG, "%s processer flush data",
           stream->stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
      stream->processer_->flush();
      stream->need_flush_ = false;
    }
  }
  LOGE(TAG, "%s process thread exit!!!",
       stream->stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
  return nullptr;
}
