
#include <functional>

#include "log/ddup_log.h"
#include "player/demux.h"

#define TAG "Stream"

Stream::Stream(stream_type_t type, EventListener *listener)
    : EventListener("Stream"),
      BufferConsumer(type == AUDIO_STREAM ? "audio_stream" : "video_Stream") {
  eos_ = false;
  stream_type_ = type;
  dec_thread_exit_ = false;
  processer_ = nullptr;
  need_flush_ = false;
  stream_on_ = false;
  listener_ = listener;
  pthread_cond_init(&cond_, NULL);
  pthread_mutex_init(&mutex_, NULL);
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
  pthread_cond_destroy(&cond_);
  pthread_mutex_destroy(&mutex_);
};

int Stream::init() {
  create_decoder();
  create_processer();
  decoder_->open();
  processer_->init();
  process_thread_ = std::thread(std::bind(&Stream::process_thread, this));

  return 0;
}

int Stream::uninit() {
  LOGE(TAG, "%s stream uninit",
       stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
  dec_thread_exit_ = true;
  // exit wait first
  pthread_mutex_lock(&mutex_);
  pthread_cond_signal(&cond_);
  pthread_mutex_unlock(&mutex_);
  // abort consume buffer from input fifo
  consume_abort(stream_type_ == AUDIO_STREAM ? AUDIO_FIFO : VIDEO_FIFO);
  // abort append buffer to raw fifo
  processer_->append_abort(stream_type_ == AUDIO_STREAM ? AUDIO_FIFO
                                                        : VIDEO_FIFO);
  if (process_thread_.joinable()) process_thread_.join();
  LOGE(TAG, "%s stream unint, processer unint",
       stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
  processer_->uninit();
  decoder_->close();
  return 0;
}

int Stream::stream_on() {
  LOGE(TAG, "%s stream on", stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
  stream_on_ = true;
  pthread_mutex_lock(&mutex_);
  pthread_cond_signal(&cond_);
  pthread_mutex_unlock(&mutex_);
  return 0;
}

int Stream::stream_off() {
  LOGE(TAG, "%s stream off", stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
  stream_on_ = false;
  return 0;
}

int Stream::check_wait() {
  bool eos_wait =
      eos_ &&
      is_fifo_empty(stream_type_ == AUDIO_STREAM ? AUDIO_FIFO : VIDEO_FIFO);
  if (eos_wait) {
    processer_->set_eos();
  }
  if (!stream_on_ || eos_wait) {
    if (eos_wait) {
      LOGE(TAG, "%s eos, wait",
           stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
    } else {
      LOGE(TAG, "%s wait stream on",
           stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
    }
    pthread_mutex_lock(&mutex_);
    pthread_cond_wait(&cond_, &mutex_);
    pthread_mutex_unlock(&mutex_);
    if (eos_wait) {
      LOGE(TAG, "%s eos, wait finish",
           stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
      eos_ = false;
    } else {
      LOGE(TAG, "%s wait stream on finish",
           stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
    }
  }
  return 0;
}

int Stream::flush() {
  fifo_type_t type = stream_type_ == AUDIO_STREAM ? AUDIO_FIFO : VIDEO_FIFO;
  LOGE(TAG, "%s stream flush", type == AUDIO_FIFO ? "AUDIO" : "VIDEO");
  discard_buffer(type);
  need_flush_ = true;
  consume_abort(type);
  processer_->append_abort(type);
  usleep(1000);  // FIXME: discard data of raw fifo first
  return 0;
}

int Stream::set_speed(float speed) {
  if (speed == 0.0) {
    stream_off();
  } else {
    stream_on();
  }
  return 0;
}

void Stream::set_eos() {
  fifo_type_t type = stream_type_ == AUDIO_STREAM ? AUDIO_FIFO : VIDEO_FIFO;
  LOGI(TAG, "%s got eos cmd", type == AUDIO_FIFO ? "AUDIO" : "VIDEO");
  eos_ = true;
  consume_abort(type);
}

int Stream::process_raw_data(void *data, void *handle) {
  Stream *stream = (Stream *)handle;
  if (stream->processer_) {
    LOGD(TAG, "process data %p", data);
    stream->processer_->process_data(data);
  }
  return 0;
}

void Stream::process_thread(void) {
  int ret = 0;
  void *data = NULL;
  void *out = NULL;
  while (!dec_thread_exit_) {
    check_wait();
    data = nullptr;
    bool status = consume_buffer(
        &data, stream_type_ == AUDIO_STREAM ? AUDIO_FIFO : VIDEO_FIFO);
    if (!status) {
      LOGE(TAG, "%s processer consume data error or abort",
           stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
      if (need_flush_) {
        LOGE(TAG, "%s processer & decoder flush data",
             stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
        processer_->flush();
        decoder_->flush();
        need_flush_ = false;
      }
      continue;
    }
    if (data == nullptr) {
      LOGE(TAG, "%s processer consume null pkt",
           stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
    }
    ret = decoder_->decode(data, &Stream::process_raw_data, this);
    if (ret < 0 && ret != DECODE_ERROR_EOS && ret != DECODE_ERROR_EAGAIN) {
      LOGE(TAG, "Decode error %d,", ret);
    }
  }
  LOGE(TAG, "%s process thread exit!!!",
       stream_type_ == AUDIO_STREAM ? "AUDIO" : "VIDEO");
}
