#include "player/demux.h"

#include <cassert>
#include <functional>

#include "log/ddup_log.h"

#define TAG "Demux"

Demux::Demux(EventListener *listener)
    : BufferProducer("demux"), EventListener("Demux") {
  LOGI(TAG, "%s", "construct Demux begin");
  video_stream_ = nullptr;
  audio_stream_ = nullptr;
  input_thread_exit_ = false;
  listener_ = listener;
  listener_->print_name();
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
  if (audio_fifo_) {
    assert(audio_fifo_->is_empty());
    delete audio_fifo_;
  }
  if (video_fifo_) {
    assert(video_fifo_->is_empty());
    delete video_fifo_;
  }
  pthread_cond_destroy(&cond_);
  pthread_mutex_destroy(&mutex_);
  pthread_mutex_destroy(&cmd_mutex_);
}

void Demux::notify_event(int event_type, void *ret) {
  listener_->notify_event(event_type, ret);
}

void Demux::notify_error(int error_type) {
  listener_->notify_error(error_type);
}

void *Demux::input_thread(void *arg) {
  LOGI(TAG, "%s", "input thread enter !!!");

  Demux *demux = (Demux *)arg;

  while (!demux->input_thread_exit_) {
    check_wait_ready();
    av_data_s data;
    LOGD(TAG, "%s", "input thread read data !!!");
    int ret = demux->read_input(&data);
    if (ret > 0) {
      if (ret == DEMUX_EOS) {
        if (demux->audio_stream_) demux->audio_stream_->set_eos();
        if (demux->video_stream_) demux->video_stream_->set_eos();
        LOGE(TAG, "%s", "input thread read eos");
        demux->set_state(DEMUX_STATE_EOS);
      } else if (ret == DEMUX_EAGAN) {
        LOGE(TAG, "%s", "input thread eagain");
      } else {
        LOGE(TAG, "input thread read data error:%d", ret);
        demux->notify_error((int)ret);
      }
      continue;
    }

    if (data.type_ == VIDEO_STREAM) {
      LOGD(TAG, "input thread append video pkt:%p", data.data);
      demux->append_buffer(&data.data, VIDEO_FIFO);
    } else {
      LOGD(TAG, "input thread append audio pkt:%p", data.data);
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
    LOGI(TAG, "%s", "intput thread wake up from waiting ready.....");
  }
}

void Demux::set_ready() {
  LOGI(TAG, "%s", "intput thread set ready.....");
  pthread_mutex_lock(&mutex_);
  pthread_cond_signal(&cond_);
  pthread_mutex_unlock(&mutex_);
}

void Demux::set_state(demux_state_t state) {
  LOGI(TAG, "set state from:%s to:%s.", get_state_string(state_),
       get_state_string(state));
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

int Demux::prepare(const char *url) {
  set_state(DEMUX_STATE_PREPARE);
  notify_event(DDUP_EVENT_DURATION, (void *)&duration_);
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
    ret = audio_stream_->init();
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
    ret = video_stream_->init();
    if (ret < 0) {
      LOGI(TAG, "%s", "video stream open failed");
      return ret;
    }
  }
  set_ready();
  return 0;
}

int Demux::set_speed(float speed) {
  LOGI(TAG, "set speed:%f", speed);
  int ret = 0;
  if (speed == 0.0) {
    set_state(DEMUX_STATE_PAUSE);
  } else {
    set_state(DEMUX_STATE_PLAY);
  }
  if (audio_stream_) audio_stream_->set_speed(speed);
  if (video_stream_) video_stream_->set_speed(speed);
  return 0;
}

int Demux::read_data_abort() {
  if (audio_stream_) append_abort(AUDIO_FIFO);
  if (video_stream_) append_abort(VIDEO_FIFO);
  return 0;
}

int Demux::read_input(av_data_s *data) {
  AutoLock lock(&cmd_mutex_);
  return read_input_impl(data);
}

int Demux::seek(long long seek_time) {
  AutoLock lock(&cmd_mutex_);
  read_data_abort();
  set_state(DEMUX_STATE_SEEK);
  LOGI(TAG, "seek:%lld, audio stream flush", seek_time);
  if (audio_stream_) audio_stream_->flush();
  LOGI(TAG, "seek:%lld, video stream flush", seek_time);
  if (video_stream_) video_stream_->flush();
  LOGI(TAG, "seek:%lld", seek_time);
  seek_impl(seek_time);
  set_state(DEMUX_STATE_PLAY);
  set_ready();
  return 0;
}

int Demux::stop() {
  LOGI(TAG, "%s", "enter demux stop");
  int ret = 0;
  read_data_abort();
  set_state(DEMUX_STATE_STOP);
  if (audio_stream_) {
    audio_stream_->flush();
    ret = audio_stream_->uninit();
    if (ret < 0) {
      LOGI(TAG, "%s", "audio stream off failed");
      return ret;
    }
  }
  if (video_stream_) {
    video_stream_->flush();
    ret = video_stream_->uninit();
    if (ret < 0) {
      LOGI(TAG, "%s", "video stream off failed");
      return ret;
    }
  }
  LOGI(TAG, "%s", "leave demux stop end");
  return 0;
}

int Demux::close() {
  LOGI(TAG, "%s", "close");
  input_thread_exit_ = true;
  set_state(DEMUX_STATE_CLOSE);
  set_ready();
  if (input_thread_.joinable()) {
    input_thread_.join();
  }
  LOGI(TAG, "%s", "close successfully");
  return 0;
}

const char *Demux::get_state_string(demux_state_t state) {
  const char *name;
  switch (state) {
    case DEMUX_STATE_OPEN:
      name = "OPEN";
      break;
    case DEMUX_STATE_PREPARE:
      name = "PREPARE";
      break;
    case DEMUX_STATE_PLAY:
      name = "PLAY";
      break;
    case DEMUX_STATE_PAUSE:
      name = "PAUSE";
      break;
    case DEMUX_STATE_SEEK:
      name = "SEEK";
      break;
    case DEMUX_STATE_STOP:
      name = "STOP";
      break;
    case DEMUX_STATE_CLOSE:
      name = "CLOSE";
      break;
    case DEMUX_STATE_EOS:
      name = "EOS";
      break;
    default:
      name = "UNKNOWN";
      break;
  }
  return name;
}
