#include "gui_widget/sdl_impl/sdl_progress_bar.h"

#include <iostream>

#include "gui_widget/sdl_impl/sdl_user_event.h"
#include "log/ddup_log.h"

#define TAG "SdlProgBar"

using namespace std;

SdlProgBar::SdlProgBar(const char *name, kiss_window *win,
                       SDL_Renderer *renderer)
    : ProgBar(name) {
  window_ = win;
  renderer_ = renderer;
  dirty_ = true;
  state_ = PLAYBACK_PLAY;
  kiss_progressbar_new(&bar_, win, win->rect.x,
                       win->rect.y + kiss_screen_height - 20,
                       kiss_screen_width);
  rect_.x = bar_.rect.x;
  rect_.y = bar_.rect.y;
  rect_.w = bar_.rect.w;
  rect_.h = bar_.rect.h;
  kiss_label_new(&time_label_, win, (char *)"0:0:0", win->rect.x,
                 win->rect.y + win->rect.h - 18);
  kiss_label_new(&dur_label_, win, (char *)"0:0:0", win->rect.w - 55,
                 win->rect.h - 18);
  duration_ = 0;
}

bool SdlProgBar::is_dirty() { return dirty_; }

void SdlProgBar::set_duration(long long duration) {
  LOGI(TAG, "set duration:%lld", duration);
  duration_ = duration;
  int hour = (duration_ / 1000) / 3600;
  int min = ((duration_ / 1000) % 3600) / 60;
  int sec = ((duration_ / 1000) % 3600) % 60;
  char buf[10];
  snprintf(buf, 10, "%d:%d:%d", hour, min, sec);
  kiss_string_copy(dur_label_.text, KISS_MAX_LABEL, buf, NULL);
  bar_.step = (1 / (float)duration_);
  LOGI(TAG, "bar step:%f", bar_.step);
  bar_.run = 1;
}

void SdlProgBar::set_current_time(long long current) {
  char buf[10];
  if (current <= 0 || current == current_time_) {
    return;
  }
  current_time_ = current;
  int hour = (current_time_ / 1000) / 3600;
  int min = ((current_time_ / 1000) % 3600) / 60;
  int sec = ((current_time_ / 1000) % 3600) % 60;
  snprintf(buf, 10, "%d:%d:%d", hour, min, sec);
  kiss_string_copy(time_label_.text, KISS_MAX_LABEL, buf, NULL);
  bar_.fraction = current_time_ / (float)duration_ + 0.01;
}

int SdlProgBar::draw() {
  kiss_progressbar_draw(&bar_, renderer_);
  kiss_label_draw(&time_label_, renderer_);
  kiss_label_draw(&dur_label_, renderer_);
  return 0;
}

int SdlProgBar::get_type() { return 0; }

int SdlProgBar::event_handler(void *event) {
  SDL_Event *e = (SDL_Event *)event;
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    // check if mouse down point is outside of progress bar, then play or pause
    PLAYBACK_STATE_E pre_state = state_;
    if (!kiss_pointinrect(e->button.x, e->button.y, &rect_)) {
      if (state_ == PLAYBACK_PLAY) {
        state_ = PLAYBACK_PAUSE;
      } else if (state_ == PLAYBACK_PAUSE) {
        state_ = PLAYBACK_PLAY;
      }
    } else {
      seek_time_ = (e->button.x / (float)window_->rect.w) * duration_;
      set_current_time(seek_time_);
      state_ = PLAYBACK_SEEK;
    }
    dirty_ = true;
    if (action_cb_) {
      action a;
      a.state = state_;
      a.seek_time = seek_time_;
      action_cb_(user_data_, &a);
    }
    if (state_ == PLAYBACK_SEEK) {
      state_ = pre_state;
    }
    return 1;
  } else if (e->type == SDL_USER_EVENT_DURATION_UPDATE) {
    long long duration = *(long long *)e->user.data1;
    set_duration(duration);
    return 1;
  } else if (e->type == SDL_USER_EVENT_POSITION_UPDATE) {
    long long position = *(long long *)e->user.data1;
    set_current_time(position);
    return 1;
  }
  return 0;
}

int SdlProgBar::set_event_resp_area(int x, int y, int w, int h) { return 0; }

void *SdlProgBar::get_window() { return window_; }

void *SdlProgBar::get_renderer() { return renderer_; }
