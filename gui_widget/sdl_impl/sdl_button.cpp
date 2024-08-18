
#include "gui_widget/sdl_impl/sdl_button.h"

#include <iostream>

#include "gui_widget/sdl_impl/sdl_user_event.h"
#include "log/ddup_log.h"

#define BUTTON_TYPE BUTTON_BASE_TYPE + 1
using namespace std;

#define TAG "SdlButton"

void SdlButton::delay_hide_timer_handler() {
  LOGI(TAG, "delay handler, button name:%s,set show:%d", name_.c_str(), false);
  set_show(false, 0);
  SDL_Event event;
  event.type = SDL_USER_EVENT_REFRESH;
  event.user.data1 = NULL;
  event.user.data2 = NULL;
  SDL_PushEvent(&event);
}

SdlButton::SdlButton(const char *name, const char *skin, SDL_Renderer *renderer)
    : Button(0, 0, 0, 0, skin, name) {
  renderer_ = renderer;
  dirty_ = false;
  hide_delay_time_ = 0;
  image_ = new SdlImage(renderer_, skin);
  width_ = image_->get_width();
  height_ = image_->get_height();
  pos_x_ = 0;
  pos_y_ = 0;
  rect_ = {0, 0, width_, height_};
  resp_rect_ = {pos_x_, pos_y_, width_, height_};
}

SdlButton::SdlButton(const char *name, const char *skin, SDL_Renderer *renderer,
                     int x, int y, int w, int h)
    : Button(x, y, w, h, skin, name) {
  renderer_ = renderer;
  dirty_ = false;
  image_ = new SdlImage(renderer_, skin);
  pos_x_ = x;
  pos_y_ = y;
  rect_ = {0, 0, w, h};
  resp_rect_ = {pos_x_, pos_y_, image_->get_width(), image_->get_height()};
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
  ret = image_->render_image(&rect_, pos_x_, pos_y_);
  if (!ret) {
    if (hide_delay_time_ > 0) {
      LOGI(TAG, "button:%s draw, start timer, delay:%d", name_.c_str(),
           hide_delay_time_);
      timer_.run(hide_delay_time_,
                 std::bind(&SdlButton::delay_hide_timer_handler, this));
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
        point_in_rect(e->button.x, e->button.y, &resp_rect_)) {
      action_cb_(user_data_, event);
      LOGI(TAG, "%s", "button handle inner click");
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

int SdlButton::get_type() { return BUTTON_TYPE; }

void *SdlButton::get_window() { return nullptr; }

void *SdlButton::get_renderer() { return (void *)renderer_; }
