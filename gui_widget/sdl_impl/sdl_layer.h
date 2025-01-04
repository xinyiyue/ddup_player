#ifndef __KISS_LAYER__
#define __KISS_LAYER__

#include <iostream>
#include <list>
#include <string>

#include "gui_widget/base/layer.h"
#include "log/ddup_log.h"

class SdlLayer : public Layer {
 public:
  SdlLayer(const char *name) : Layer(name){};
  virtual int event_handler(void *event) final {
    bool catched = false;
    SDL_Event *e = (SDL_Event *)event;
    for (auto widget : widget_list_) {
      catched = widget->event_handler(event);
      if (catched) {
        LOGD("SdlLayer", "layer:%s ,widget:%s, cached event:%d", name_.c_str(),
             widget->name_.c_str(), e->type);
        break;
      }
    }
    return catched;
  };

  virtual int update() final {
    for (auto widget : widget_list_) {
      LOGD("SdlLayer", "widget:%s, is_dirty:%d, %p", widget->name_.c_str(),
           widget->dirty_, &widget->dirty_);
      if (widget->dirty_) widget->draw();
    }
    return 0;
  };
};

#endif
