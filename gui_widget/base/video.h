#ifndef __VIDEO_H_H__
#define __VIDEO_H_H__

#include "gui_widget/base/widget.h"

class Video : public Widget {
 public:
  Video(const char *name) : Widget(name){};
  virtual ~Video(){};

  virtual int open(const char *url) = 0;
  virtual int set_speed(float speed) = 0;
  virtual int stop() = 0;
  virtual int seek(long long seek_time) = 0;
  virtual int close() = 0;

  virtual int draw() { return 0; };
  virtual int get_type() { return 0; };
  virtual int event_handler(void *event) { return 0; };
  virtual int set_event_resp_area(int x, int y, int w, int h) { return 0; };
  virtual void *get_window() { return nullptr; };
  virtual void *get_renderer() { return nullptr; };
};

#endif
