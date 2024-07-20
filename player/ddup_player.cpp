#include "player/ddup_player.h"

#include <iostream>

#include "log/ddup_log.h"
#include "player/component.h"

using namespace std;

#define TAG "DDupPlayer"

DDupPlayer::DDupPlayer(EventListener *listener) : EventListener("DDupPlayer") {
  listener_ = listener;
}

DDupPlayer::~DDupPlayer() {
  if (pipeline_) {
    delete pipeline_;
    pipeline_ = nullptr;
  }
}

int DDupPlayer::open() {
  int ret = CreatePipeline(static_cast<EventListener *>(this), &pipeline_);
  if (ret < 0) {
    LOGE(TAG, "create pipeline failed:%d", ret);
    return ret;
  }
  pipeline_->open();
  return 0;
}

int DDupPlayer::prepare(const char *url) {
  pipeline_->prepare(url);
  return 0;
}

int DDupPlayer::set_speed(float speed) {
  pipeline_->set_speed(speed);
  return 0;
}

int DDupPlayer::seek(long long seek_time) {
  pipeline_->seek(seek_time);
  return 0;
}

int DDupPlayer::stop() {
  pipeline_->stop();
  return 0;
}

int DDupPlayer::close() {
  pipeline_->close();
  return 0;
}

long long DDupPlayer::get_current_time() { return current_time_; }

void DDupPlayer::notify_event(int event_type, void *ret) {
  ddup_event_t event = (ddup_event_t)event_type;
  switch (event) {
    case DDUP_EVENT_OPENED:
    case DDUP_EVENT_PLAYED:
    case DDUP_EVENT_PAUSED:
    case DDUP_EVENT_SEEKED:
    case DDUP_EVENT_STOPPED:
    case DDUP_EVENT_CLOSED:
      LOGI(TAG, "got playstate event:%d, ret:%d", event, *(int *)ret);
      break;
    case DDUP_EVENT_CURRENT_TIME:
      current_time_ = *(long long *)ret;
      LOGI(TAG, "got current time:%lld", current_time_);
      break;
    default:
      break;
  };
}

void DDupPlayer::notify_error(int error_type) {
  LOGI(TAG, "got error:%d", error_type);
  listener_->notify_error((ddup_error_t)error_type);
}
