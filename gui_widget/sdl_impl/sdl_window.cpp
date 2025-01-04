
#include "gui_widget/sdl_impl/sdl_window.h"

#include <iostream>
#include <ostream>

#include "gui_widget/sdl_impl/sdl_user_event.h"
#include "log/ddup_log.h"

#define TAG "SdlWindow"
using namespace std;

SdlWindow::SdlWindow(const char *name, int w, int h) : Window(name, w, h) {
  exit_ = false;
  renderer_mutex_ = SDL_CreateMutex();
}

SdlWindow::~SdlWindow() {
  LOGI(TAG, "SdlWindow:%s destructor", name_.c_str());
  SDL_DestroyMutex(renderer_mutex_);
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
  IMG_Quit();
  TTF_Quit();
  SDL_Quit();
}

int SdlWindow::create() {
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Rect srect;
  SDL_GetDisplayBounds(0, &srect);
  if (width_ > srect.w || height_ > srect.h) {
    width_ = srect.w;
    height_ = srect.h;
    LOGI(TAG, "ajust window size to: %d, %d", width_, height_);
  }
  IMG_Init(IMG_INIT_PNG);
  TTF_Init();
  SDL_CreateWindowAndRenderer(width_, height_, 0, &window_, &renderer_);
  LOGI(TAG, "create window and renderer, size:%d, %d", width_, height_);
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
      if (event.type == SDL_USER_EVENT_IMAGE_HIDE)
        LOGI(TAG, "get event:%d, SDL_USER_EVENT_IMAGE_HIDE",
             SDL_USER_EVENT_IMAGE_HIDE);
      if (event.type == SDL_USER_EVENT_LABEL_HIDE)
        LOGI(TAG, "get event:%d, SDL_USER_EVENT_IMAGE_HIDE",
             SDL_USER_EVENT_IMAGE_HIDE);
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

  LOGI(TAG, "window:%s eixt!!!", name_.c_str());
  return 0;
}

int SdlWindow::update() {
  AutoLock lock(renderer_mutex_);
  SDL_RenderClear(renderer_);
  for (auto layer : layer_list_) {
    if (!layer->show_) {
      continue;
    }
    layer->update();
  }
  SDL_RenderPresent(renderer_);
  return 0;
}
