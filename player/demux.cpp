#include "player/demux.h"

#include "log/ddup_log.h"

#define TAG "Demux"

Demux::Demux(EventListener *listener) {
  LOGI(TAG, "%s", "construct Demux begin");
  video_stream_ = nullptr;
  audio_stream_ = nullptr;
  input_thread_exit_ = false;
  listener_ = listener;
  url_ = "";
  ready_to_read_data_ = false;
  pthread_cond_init(&cond_, NULL);
  pthread_mutex_init(&mutex_, NULL);
  pthread_mutex_init(&cmd_mutex_, NULL);
  LOGI(TAG, "%s", "construct Demux end");
}

Demux::~Demux() {
  pthread_cond_destroy(&cond_);
  pthread_mutex_destroy(&mutex_);
  pthread_mutex_destroy(&cmd_mutex_);
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
    av_data_s data;
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
        LOGI(TAG, "input thread append video pkt:%p", data.data);
        demux->video_stream_->append_data(&data.data);
      } else {
        LOGI(TAG, "input thread append audio pkt:%p", data.data);
        demux->audio_stream_->append_data(&data.data);
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

void Demux::request_input_data() {
  ready_to_read_data_ = true;
  pthread_mutex_lock(&mutex_);
  pthread_cond_signal(&cond_);
  pthread_mutex_unlock(&mutex_);
}

int Demux::open() {
  LOGI(TAG, "%s", "demux open, create input thread");
  pthread_create(&input_thread_id_, NULL, &Demux::input_thread, (void *)this);
  return 0;
}

int Demux::prepare(char *url) {
  url_ = url;
  int ret = create_stream();
  if (ret < 0) {
    LOGE(TAG, "create stream failed:%d", ret);
    return ret;
  }

  if (audio_stream_) {
    ret = audio_stream_->stream_on();
    if (ret < 0) {
      LOGI(TAG, "%s", "audio stream open failed");
      return ret;
    }
  }
  if (video_stream_) {
    ret = video_stream_->stream_on();
    if (ret < 0) {
      LOGI(TAG, "%s", "video stream open failed");
      return ret;
    }
  }
  return 0;
}

int Demux::play(float speed) {
  int ret = 0;
  if (audio_stream_) {
    audio_stream_->play(speed);
    if (ret < 0) {
      LOGI(TAG, "%s", "audio stream play failed");
      return ret;
    }
  }
  if (video_stream_) {
    video_stream_->play(speed);
    if (ret < 0) {
      LOGI(TAG, "%s", "video stream play failed");
      return ret;
    }
  }
  return 0;
}

int Demux::pause() {
  int ret = 0;
  if (audio_stream_) {
    ret = audio_stream_->pause();
    if (ret < 0) {
      LOGI(TAG, "%s", "audio stream pause failed");
      return ret;
    }
  }
  if (video_stream_) {
    ret = video_stream_->pause();
    if (ret < 0) {
      LOGI(TAG, "%s", "video stream pause failed");
      return ret;
    }
  }
  return 0;
}

int Demux::seek(long long seek_time) {
  if (audio_stream_) audio_stream_->clear_data(this);
  if (video_stream_) video_stream_->clear_data(this);
  request_input_data();
  return 0;
}

int Demux::stop() {
  LOGI(TAG, "%s", "enter demux stop");
  int ret = 0;
  if (audio_stream_) {
    ret = audio_stream_->stream_off();
    if (ret < 0) {
      LOGI(TAG, "%s", "audio stream off failed");
      return ret;
    }
    LOGI(TAG, "%s", "clear audio stream data");
    audio_stream_->clear_data(this);
  }
  if (video_stream_) {
    ret = video_stream_->stream_off();
    if (ret < 0) {
      LOGI(TAG, "%s", "video stream off failed");
      return ret;
    }
    LOGI(TAG, "%s", "clear video stream data");
    video_stream_->clear_data(this);
  }

  return 0;
}

int Demux::close() {
  input_thread_exit_ = true;
  request_input_data();
  int ret = pthread_join(input_thread_id_, NULL);
  if (ret != 0) {
    LOGE(TAG, "pthread join failed:%d", ret);
    return ret;
  }
  LOGI(TAG, "%s", "input thread exit successfully");
  return 0;
}
