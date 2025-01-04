#ifndef __WIDGET__
#define __WIDGET__

#include <iostream>
#include <string>

#include "base_util/ddup_timer.h"
#include "log/ddup_log.h"

enum WIDGET_TYPE {
  BUTTON_BASE_TYPE = 0,
  BAR_BASE_TYPE = 10,
};

typedef void (*action_callback)(void *userdata, void *arg);

class Widget {
 public:
  Widget() {
    action_cb_ = nullptr;
    user_data_ = nullptr;
    hide_delay_time_ = -1;
    dirty_ = false;
  }
  Widget(const char *name) {
    action_cb_ = nullptr;
    user_data_ = nullptr;
    hide_delay_time_ = -1;
    name_ = name;
    dirty_ = false;
  }
  virtual bool is_dirty() { return dirty_; };
  virtual int draw() = 0;
  virtual int get_type() = 0;
  virtual int event_handler(void *event) = 0;
  virtual int set_event_resp_area(int x, int y, int w, int h) = 0;
  virtual void *get_window() = 0;
  virtual void *get_renderer() = 0;
  virtual void set_show(bool show, int time_ms) {
    LOGI("Widget", "widget name:%s, set show:%d, delay:%d", name_.c_str(), show,
         time_ms);
    if (show) {
      if (time_ms > 0) {
        hide_delay_time_ = time_ms;
      }
      dirty_ = true;
    } else {
      dirty_ = false;
    }
  }
  virtual void set_action_callback(action_callback cb, void *userdata) {
    action_cb_ = cb;
    user_data_ = userdata;
  };

 public:
  action_callback action_cb_;
  void *user_data_;
  bool dirty_;
  int hide_delay_time_;  // ms
  std::string name_;
  DDupTimer timer_;
};

#endif
