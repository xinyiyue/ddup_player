#ifndef __EVENT_LISTENER__H__
#define __EVENT_LISTENER__H__

#include <string>

#include "log/ddup_log.h"

typedef enum DDUP_EVENT_TYPE {
  DDUP_EVENT_DURATION,
  DDUP_EVENT_EOS,
  DDUP_EVENT_POSITION,
  DDUP_EVENT_AUDIO_EOS,
  DDUP_EVENT_VIDEO_EOS,
  DDUP_EVENT_OPENED,
  DDUP_EVENT_PLAYED,
  DDUP_EVENT_PAUSED,
  DDUP_EVENT_SEEKED,
  DDUP_EVENT_STOPPED,
  DDUP_EVENT_CLOSED,
} ddup_event_t;

typedef enum DDUP_ERROR_TYPE {
  DDUP_PLAYBACK_ERROR,
  DDUP_DECODE_ERROR,
  DDUP_NETWORK_ERROR,
  DDUP_UNKOWN_ERROR,
} ddup_error_t;

class EventListener {
 public:
  EventListener(const char *name) {
    name_ = name;
    LOGI("EventListener", "create event listener:%s", name_.c_str());
    listener_ = nullptr;
  };
  virtual ~EventListener(){};
  virtual void notify_event(int event_type, void *ret) {
    if (listener_) {
      listener_->notify_event(event_type, ret);
    }
  };
  virtual void notify_error(int error_type) {
    if (listener_) {
      listener_->notify_error(error_type);
    }
  };
  EventListener *get_top_listener() {
    LOGI("EventListener", "enter %s get_top_listener", name_.c_str());
    if (listener_) {
      return listener_->get_top_listener();
    }
    LOGI("EventListener", "%s get_top_listener,got listener:%p", name_.c_str(),
         this);
    return this;
  }
  void print_name() {
    LOGD("EventListener", "current listener name:%s", name_.c_str());
  }

 protected:
  EventListener *listener_;
  std::string name_;
};

#endif
