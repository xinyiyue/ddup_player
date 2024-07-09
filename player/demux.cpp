#include "player/demux.h"

#include <functional>

#include "log/ddup_log.h"

#define TAG "Demux"

Demux::Demux(EventListener *listener) : BufferProducer("demux") {
  LOGI(TAG, "%s", "construct Demux begin");
  video_stream_ = nullptr;
  audio_stream_ = nullptr;
  input_thread_exit_ = false;
  listener_ = listener;
  url_ = "";
  pthread_cond_init(&cond_, NULL);
  pthread_mutex_init(&mutex_, NULL);
  pthread_mutex_init(&cmd_mutex_, NULL);
  LOGI(TAG, "%s", "construct Demux end");
}

Demux::~Demux() {
  if (audio_stream_) {
    delete audio_stream_;
    audio_stream_ = nullptr;
  }
  if (video_stream_) {
    delete video_stream_;
    video_stream_ = nullptr;
  }
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

  while (!demux->input_thread_exit_) {
    demux->check_discard_data();
    demux->check_wait_ready();
    av_data_s data;
    LOGD(TAG, "%s", "input thread read data !!!");
    int ret = demux->read_input_data(&data);
    if (ret > 0) {
      if (ret == DEMUX_EOS) {
        if (demux->audio_stream_) demux->audio_stream_->set_eos();
        if (demux->video_stream_) demux->video_stream_->set_eos();
        LOGE(TAG, "%s", "input thread read eos");
        demux->set_state(DEMUX_STATE_EOS);
        continue;
      } else if (ret == DEMUX_EAGAN) {
        continue;
      } else {
        LOGE(TAG, "input thread read data error:%d", ret);
        demux->notify_error((int)ret);
        continue;
      }
    }

    if (data.type_ == VIDEO_STREAM) {
      LOGI(TAG, "input thread append video pkt:%p", data.data);
      demux->append_buffer(&data.data, VIDEO_FIFO);
    } else {
      LOGI(TAG, "input thread append audio pkt:%p", data.data);
      demux->append_buffer(&data.data, AUDIO_FIFO);
    }
  }
  LOGI(TAG, "%s", "input thread exit !!!");
  return NULL;
}

void Demux::check_wait_ready() {
  if (state_ != DEMUX_STATE_PLAY && state_ != DEMUX_STATE_PREPARE) {
    LOGI(TAG, "%s", "intput thread wait ready.....");
    pthread_mutex_lock(&mutex_);
    pthread_cond_wait(&cond_, &mutex_);
    pthread_mutex_unlock(&mutex_);
  }
}

void Demux::set_ready() {
  LOGI(TAG, "%s", "intput thread set ready.....");
  pthread_mutex_lock(&mutex_);
  pthread_cond_signal(&cond_);
  pthread_mutex_unlock(&mutex_);
}

void Demux::set_state(demux_state_t state) {
  LOGI(TAG, "set state from:%d to :%d.", state_, state);
  state_ = state;
}

int Demux::open() {
  LOGI(TAG, "%s", "demux open, create input thread");
  set_state(DEMUX_STATE_OPEN);
  input_thread_ =
      std::thread(std::bind(&Demux::input_thread, this, std::placeholders::_1),
                  (void *)this);

  return 0;
}

int Demux::prepare(char *url) {
  set_state(DEMUX_STATE_PREPARE);
  url_ = url;
  int ret = create_stream();
  if (ret < 0) {
    LOGE(TAG, "create stream failed:%d", ret);
    return ret;
  }

  if (audio_stream_) {
    audio_fifo_ = new Fifo("ainput_fifo", DEFAULT_FIFO_SIZE, sizeof(void *),
                           AUDIO_FIFO, this);
    bind_fifo(audio_fifo_);
    audio_stream_->bind_fifo(audio_fifo_);
    ret = audio_stream_->stream_on();
    if (ret < 0) {
      LOGI(TAG, "%s", "audio stream open failed");
      return ret;
    }
  }
  if (video_stream_) {
    video_fifo_ = new Fifo("vinput_fifo", DEFAULT_FIFO_SIZE, sizeof(void *),
                           VIDEO_FIFO, this);
    bind_fifo(video_fifo_);
    video_stream_->bind_fifo(video_fifo_);
    ret = video_stream_->stream_on();
    if (ret < 0) {
      LOGI(TAG, "%s", "video stream open failed");
      return ret;
    }
  }
  set_ready();
  return 0;
}

int Demux::play(float speed) {
  int ret = 0;
  set_state(DEMUX_STATE_PLAY);
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
  set_ready();
  return 0;
}

int Demux::pause() {
  int ret = 0;
  set_state(DEMUX_STATE_PAUSE);
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

int Demux::check_discard_data() {
  if (state_ == DEMUX_STATE_STOP || state_ == DEMUX_STATE_PLAY_SEEK ||
      state_ == DEMUX_STATE_PAUSE_SEEK) {
    LOGI(TAG, "%s", "input thread discard data");
    if (audio_stream_) audio_stream_->discard_buffer(AUDIO_FIFO);
    if (video_stream_) video_stream_->discard_buffer(VIDEO_FIFO);

    if (state_ == DEMUX_STATE_PLAY_SEEK) {
      state_ = DEMUX_STATE_PLAY;
    } else if (state_ = DEMUX_STATE_PAUSE_SEEK) {
      state_ = DEMUX_STATE_PAUSE;
    }
  }

  return 0;
}

int Demux::read_data_abort() {
  if (audio_stream_) append_abort(AUDIO_FIFO);
  if (video_stream_) append_abort(VIDEO_FIFO);
  return 0;
}

int Demux::seek(long long seek_time) {
  LOGI(TAG, "seek:%lld", seek_time);
  demux_state_t pre_state = state_;
  if (pre_state == DEMUX_STATE_PLAY) {
    set_state(DEMUX_STATE_PLAY_SEEK);
  } else if (pre_state = DEMUX_STATE_PAUSE) {
    set_state(DEMUX_STATE_PAUSE_SEEK);
  }
  read_data_abort();
  LOGI(TAG, "seek:%lld, audio stream flush", seek_time);
  if (audio_stream_) audio_stream_->flush();
  LOGI(TAG, "seek:%lld, video stream flush", seek_time);
  if (video_stream_) video_stream_->flush();
  return 0;
}

int Demux::stop() {
  LOGI(TAG, "%s", "enter demux stop");
  int ret = 0;
  set_state(DEMUX_STATE_STOP);
  set_ready();
  read_data_abort();
  if (audio_stream_) {
    ret = audio_stream_->stream_off();
    if (ret < 0) {
      LOGI(TAG, "%s", "audio stream off failed");
      return ret;
    }
  }
  if (video_stream_) {
    ret = video_stream_->stream_off();
    if (ret < 0) {
      LOGI(TAG, "%s", "video stream off failed");
      return ret;
    }
  }
  LOGI(TAG, "%s", "leave demux stop end");
  return 0;
}

int Demux::close() {
  LOGI(TAG, "%s", "close, to exit input thread");
  set_state(DEMUX_STATE_CLOSE);
  input_thread_exit_ = true;
  set_ready();
  if (input_thread_.joinable()) {
    input_thread_.join();
  }
  LOGI(TAG, "%s", "input thread exit successfully");
  return 0;
}
