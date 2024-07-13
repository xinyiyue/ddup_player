#ifndef __BUTTON__
#include <string>

#include "gui_widget/base/widget.h"

class Button : public Widget {
 public:
  int pos_x_;
  int pos_y_;
  int width_;
  int height_;
  std::string skin_file_;
  Button(int x, int y, int w, int h, const char *skin) : Widget() {
    pos_x_ = x;
    pos_y_ = y;
    width_ = w;
    height_ = h;
    skin_file_ = skin;
  };

  Button(int x, int y, int w, int h, const char *skin, const char *name)
      : Widget(name) {
    pos_x_ = x;
    pos_y_ = y;
    width_ = w;
    height_ = h;
    skin_file_ = skin;
  };

  virtual ~Button(){};
  virtual int draw() { return 0; };
  virtual bool is_dirty() { return true; }
  virtual int get_type() { return BUTTON_BASE_TYPE; };
  virtual void *get_window() { return nullptr; };
  virtual void *get_renderer() { return nullptr; };
  virtual int event_handler(void *event) { return 0; };
  virtual int set_event_resp_area(int x, int y, int w, int h) { return 0; };
};
#endif
