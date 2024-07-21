
#include "gui_widget/sdl_impl/sdl_window.h"

#include <iostream>
#include <ostream>

#include "log/ddup_log.h"

#define TAG "SdlWindow"
using namespace std;

SdlWindow::SdlWindow(const char *name, int w, int h) : Window(name, w, h) {
  exit_ = false;
}

SdlWindow::~SdlWindow() {}

int SdlWindow::create() {
  renderer_ = kiss_init((char *)name_.c_str(), &array_, width_, height_);
  if (!renderer_) return 1;
  kiss_window_new(&window_, NULL, 0, 0, 0, width_, height_);
  window_.visible = 1;
  return 0;
}

int SdlWindow::event_handler(void *event) {
  bool catched = false;
  for (auto layer : layer_list_) {
    if (!layer->show_) {
      continue;
    }
    catched = layer->event_handler(event);
    if (catched) {
      break;
    }
  }
  return catched;
}

int SdlWindow::show() {
  LOGI(TAG, "window:%s show !!", name_.c_str());
  update();
  SDL_Event event;
  while (!exit_) {
    while (SDL_WaitEvent(&event)) {
      if (event_handler(&event)) {
        update();
      }

      if (event.type == SDL_QUIT) {
        LOGI(TAG, "%s", "get quit event");
        exit_ = true;
        break;
      }
    }
  }

  kiss_clean(&array_);
  LOGI(TAG, "window:%s eixt!!!", name_.c_str());
  return 0;
}

int SdlWindow::update() {
  SDL_RenderClear(renderer_);
  kiss_window_draw(&window_, renderer_);
  for (auto layer : layer_list_) {
    if (!layer->show_) {
      continue;
    }
    layer->update();
  }
  SDL_RenderPresent(renderer_);
  return 0;
}
