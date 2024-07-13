
#include "gui_widget/sdl_impl/sdl_button.h"

#include <iostream>

#include "gui_widget/sdl_impl/sdl_user_event.h"
#include "log/ddup_log.h"

#define KISS_BUTTON_TYPE BUTTON_BASE_TYPE + 1
using namespace std;

#define TAG "SdlButton"

unsigned int delay_hide_timer_handler(unsigned int interval, void *param) {
  SdlButton *button = (SdlButton *)param;
  SDL_RemoveTimer(button->hide_delay_timerid_);
  button->hide_delay_timerid_ = 0;
  LOGI(TAG, "delay handler, button name:%s,set show:%d", button->name_.c_str(),
       false);
  button->set_show(false, 0);
  SDL_Event event;
  event.type = SDL_USER_EVENT_REFRESH;
  event.user.data1 = NULL;
  event.user.data2 = NULL;
  SDL_PushEvent(&event);
  return 0;
}

SdlButton::SdlButton(const char *name, const char *skin, kiss_array *arr,
                     kiss_window *win, SDL_Renderer *renderer)
    : Button(0, 0, 0, 0, skin, name) {
  renderer_ = renderer;
  window_ = win;
  dirty_ = false;
  hide_delay_timerid_ = -1;
  kiss_image_new(&image_, (char *)skin, arr, renderer);
  pos_x_ = window_->rect.w / 2 - image_.w / 2;
  pos_y_ = window_->rect.h / 2 - image_.h / 2;
  width_ = image_.w;
  height_ = image_.h;
  kiss_makerect(&rect_, 0, 0, image_.w, image_.h);
  kiss_makerect(&resp_rect_, pos_x_, pos_y_, image_.w, image_.h);
}

SdlButton::SdlButton(const char *name, const char *skin, kiss_array *arr,
                     kiss_window *win, SDL_Renderer *renderer, int x, int y,
                     int w, int h)
    : Button(x, y, w, h, skin, name) {
  renderer_ = renderer;
  window_ = win;
  kiss_image_new_scaled(&image_, (char *)skin, arr, w, h, renderer_);
  pos_x_ = x;
  pos_y_ = y;
  kiss_makerect(&rect_, 0, 0, w, h);
  kiss_makerect(&resp_rect_, pos_x_, pos_y_, image_.w, image_.h);
}

int SdlButton::set_event_resp_area(int x, int y, int w, int h) {
  resp_rect_.x = x;
  resp_rect_.y = y;
  resp_rect_.w = w;
  resp_rect_.h = h;
  return 0;
}

int SdlButton::draw() {
  int ret = 0;
  ret = kiss_renderimage(renderer_, image_, pos_x_, pos_y_, &rect_);
  if (!ret) {
    if (hide_delay_time_ > 0) {
      LOGI(TAG, "button:%s draw, start timer, delay:%d", name_.c_str(),
           hide_delay_time_);
      hide_delay_timerid_ =
          SDL_AddTimer(hide_delay_time_, delay_hide_timer_handler, this);
      hide_delay_time_ = 0;
    }
  }
  return ret;
}

void SdlButton::set_show(bool show, int time_ms) {
  LOGI(TAG, "button name:%s set show:%d, delay:%d ms", name_.c_str(), show,
       time_ms);
  Widget::set_show(show, time_ms);
  SDL_Event event;
  event.type = SDL_USER_EVENT_REFRESH;
  event.user.data1 = NULL;
  event.user.data2 = NULL;
  SDL_PushEvent(&event);
}

int SdlButton::event_handler(void *event) {
  SDL_Event *e = (SDL_Event *)event;
  if (action_cb_) {
    if (dirty_ && e->type == SDL_MOUSEBUTTONDOWN &&
        kiss_pointinrect(e->button.x, e->button.y, &resp_rect_)) {
      action_cb_(user_data_, event);
      LOGI(TAG, "%s", "kiss_button handle inner click");
      return 1;
    }
  }
  if (e->type == SDL_USER_EVENT_REFRESH) {
    LOGI(TAG, "%s", "catch event 100, force update button");
    return 1;
  }

  return 0;
}

bool SdlButton::is_dirty() { return dirty_; }

int SdlButton::get_type() { return KISS_BUTTON_TYPE; }

void *SdlButton::get_window() { return (void *)window_; }

void *SdlButton::get_renderer() { return (void *)renderer_; }
