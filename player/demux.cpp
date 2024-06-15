#include "player/demux.h"

#include "log/ddup_log.h"

#define TAG "Demux"

Demux::Demux(EventListener *listener, char *url) {
  video_stream_ = nullptr;
  audio_stream_ = nullptr;
  input_thread_exit_ = false;
  listener_ = listener;
  url_ = url;
  ready_to_read_data_ = false;
  pthread_cond_init(&cond_, NULL);
  pthread_mutex_init(&mutex_, NULL);
}

Demux::~Demux() {
  pthread_cond_destroy(&cond_);
  pthread_mutex_destroy(&mutex_);
}

void Demux::notify_event(int event_type, void *ret) {}

void Demux::notify_error(int error_type) {
  listener_->notify_error(error_type);
}

void *Demux::input_thread(void *arg) {
  LOGI(TAG, "%s", "input thread enter !!!");

  Demux *demux = (Demux *)arg;
  bool audio_writeable;
  bool video_writeable;

  while (!demux->input_thread_exit_) {
    audio_writeable = false;
    video_writeable = false;
    if ((demux->audio_stream_ && !demux->audio_stream_->is_fifo_full()) ||
        !demux->audio_stream_) {
      audio_writeable = true;
    }
    if ((demux->video_stream_ && !demux->video_stream_->is_fifo_full()) ||
        !demux->video_stream_) {
      video_writeable = true;
    }
    if (audio_writeable && video_writeable && demux->ready_to_read_data_) {
      av_data_s data;
      int ret = demux->read_input_data(&data);
      if (ret > 0) {
        if (ret == DEMUX_EOS) {
          if (demux->audio_stream_) demux->audio_stream_->set_eos();
          if (demux->video_stream_) demux->video_stream_->set_eos();
        } else if (ret == DEMUX_EAGAN) {
          continue;
        } else {
          demux->notify_error((int)ret);
          continue;
        }
      }
      if (data.type_ == VIDEO_STREAM) {
        LOGI(TAG, "%s", "input thread got video pkt");
        demux->video_stream_->append_data((void *)&data);
      } else {
        LOGI(TAG, "%s", "input thread got video pkt");
        demux->audio_stream_->append_data((void *)&data);
      }
    } else {
      pthread_mutex_lock(&demux->mutex_);
      pthread_cond_wait(&demux->cond_, &demux->mutex_);
      pthread_mutex_unlock(&demux->mutex_);
      LOGI(TAG, "%s", "input thread wake up !!!");
    }
  };
  LOGI(TAG, "%s", "input thread exit !!!");
  return NULL;
}

int Demux::init() {
  pthread_create(&input_thread_id_, NULL, &Demux::input_thread, (void *)this);
  return 0;
}

void Demux::request_input_data() {
  ready_to_read_data_ = true;
  pthread_mutex_lock(&mutex_);
  pthread_cond_signal(&cond_);
  pthread_mutex_unlock(&mutex_);
}
