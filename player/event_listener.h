#ifndef __EVENT_LISTENER__H__
#define __EVENT_LISTENER__H__

#include <string>

#include "log/ddup_log.h"

class EventListener {
 public:
  EventListener(const char *name) {
    name_ = name;
    LOGI("EventListener", "create event listener:%s", name_.c_str());
    listener_ = nullptr;
  };
  virtual ~EventListener(){};
  virtual void notify_event(int event_type, void *ret){};
  virtual void notify_error(int error_type){};
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
    LOGI("EventListener", "current listener name:%s", name_.c_str());
  }

 protected:
  EventListener *listener_;
  std::string name_;
};

#endif
