#ifndef __WINDOW__
#define __WINDOW__

#include <string.h>

#include <algorithm>
#include <list>
#include <string>

#include "layer.h"

class Window {
 public:
  Window(const char *name, int w, int h) {
    name_ = name;
    width_ = w;
    height_ = h;
  };
  virtual int create() = 0;
  virtual int update() = 0;
  virtual int show() = 0;

  virtual int add_layer(Layer *layer) {
    if (layer_list_.size() == 0) {
      layer_list_.insert(layer_list_.begin(), layer);
    } else {
      std::list<Layer *>::iterator it = layer_list_.begin();
      while (it != layer_list_.end()) {
        if ((*it)->zorder_ > layer->zorder_) {
          layer_list_.insert(it, layer);
          break;
        }
        it++;
      }
    }
    return 0;
  };

  virtual int remove_layer(Layer *layer) {
    auto it = std::find(layer_list_.begin(), layer_list_.end(), layer);
    if (it != layer_list_.end()) {
      layer_list_.erase(it);
    }
    return 0;
  };

  virtual Layer *get_layer_by_name(char *name) {
    Layer *layer = nullptr;
    for (auto it : layer_list_) {
      if (!strcmp((char *)it->name_.c_str(), name)) {
        layer = it;
        break;
      }
    }
    return layer;
  };
  int get_window_info(int *w, int *h) {
    *w = width_;
    *h = height_;
    return 0;
  };

 public:
  int width_;
  int height_;
  std::string name_;
  std::list<Layer *> layer_list_;
};
#endif
