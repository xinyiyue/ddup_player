#ifndef __LAYER__
#define __LAYER__

#include <list>
#include <string>

#include "gui_widget/base/widget.h"

class Layer {
 public:
  Layer(const char *name) { name_ = name; }
  int add_widget(Widget *widget) {
    std::list<Widget *>::iterator it = widget_list_.end();
    widget_list_.insert(it, widget);
    return 0;
  };
  int remove_widget();
  int set_show(bool show) {
    show_ = show;
    return 0;
  };
  int set_zorder(int zorder) {
    zorder_ = zorder;
    return 0;
  }
  int get_zorder() { return zorder_; }
  virtual int event_handler(void *event) = 0;
  virtual int update() = 0;

 public:
  std::string name_;
  int zorder_;
  bool show_;
  std::list<Widget *> widget_list_;
};

#endif
