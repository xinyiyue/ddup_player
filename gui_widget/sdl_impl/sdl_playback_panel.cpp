#include "gui_widget/sdl_impl/sdl_playback_panel.h"

#include <iostream>

#include "gui_widget/sdl_impl/sdl_user_event.h"
#include "log/ddup_log.h"

#define TAG "SdlProgBar"

using namespace std;

#define DELAY_HIDE_TIME 5000

SdlPlaybackPanel::SdlPlaybackPanel(const char *name, const SDL_Rect &rect,
                                   const char *prog_skin,
                                   SDL_Renderer *renderer)
    : ProgBar(name) {
  renderer_ = renderer;
  dirty_ = true;
  state_ = PLAYBACK_PLAY;
  SDL_Color gray = {64, 64, 64, 100};
  SDL_Color white = {100, 100, 100, 0};
  s_rect_ = new SdlRect(renderer_, rect, gray, 2, gray);
  font_ = new SdlFont(renderer_, "ddup_font.ttf");
  memcpy(time_str_, "0:00:00 -", 11);
  memcpy(duration_str_, "- 0:00:00", 11);
  time_label_rect_ = font_->get_text_rect(time_str_);
  dur_label_rect_ = font_->get_text_rect(duration_str_);
  time_label_rect_.x = 0;
  time_label_rect_.y = rect.y;
  dur_label_rect_.x = rect.w - dur_label_rect_.w;
  dur_label_rect_.y = rect.y;
  bar_ = new SdlProgressBar(renderer, name, prog_skin);
  SDL_Rect bar_rect = {rect.x + time_label_rect_.w, rect.y,
                       rect.w - time_label_rect_.w - dur_label_rect_.w, rect.h};
  bar_->set_bar_rect(bar_rect, gray, 2, white);
  duration_ = 0;
  set_show(true, DELAY_HIDE_TIME);
  force_hide_ = false;
}

bool SdlPlaybackPanel::is_dirty() { return dirty_; }

void SdlPlaybackPanel::set_duration(long long duration) {
  LOGI(TAG, "set duration:%lld", duration);
  duration_ = duration;
  int hour = (duration_ / 1000) / 3600;
  int min = ((duration_ / 1000) % 3600) / 60;
  int sec = ((duration_ / 1000) % 3600) % 60;
  char buf[11];
  snprintf(buf, 11, "- %d:%2d:%2d", hour, min, sec);
  memcpy(duration_str_, buf, 11);
  double step = 1 / (double)duration_;
  bar_->set_step(step);
  LOGI(TAG, "bar step:%f", step);
  bar_->set_run(true);
}

void SdlPlaybackPanel::set_current_time(long long current) {
  char buf[11];
  if (current <= 0 || current == current_time_) {
    return;
  }
  current_time_ = current;
  int hour = (current_time_ / 1000) / 3600;
  int min = ((current_time_ / 1000) % 3600) / 60;
  int sec = ((current_time_ / 1000) % 3600) % 60;
  snprintf(buf, 11, "%d:%2d:%2d -", hour, min, sec);
  memcpy(time_str_, buf, 11);
  double fraction = current_time_ / (float)duration_ + 0.01;
  bar_->set_fraction(fraction);
}

void SdlPlaybackPanel::delay_hide_timer_handler() {
  LOGI(TAG, "delay handler, button name:%s,set show:%d", name_.c_str(), false);
  set_show(false, 0);
  SDL_Event event;
  event.type = SDL_USER_EVENT_REFRESH;
  event.user.data1 = NULL;
  event.user.data2 = NULL;
  SDL_PushEvent(&event);
}

int SdlPlaybackPanel::draw() {
  s_rect_->render_rect();
  bar_->render_bar();
  SDL_Color white = {200, 200, 200, 0};
  font_->render_text(time_str_, time_label_rect_, white);
  font_->render_text(duration_str_, dur_label_rect_, white);

  if (hide_delay_time_ > 0) {
    LOGD(TAG, "button:%s draw, start timer, delay:%d", name_.c_str(),
         hide_delay_time_);
    timer_.run(hide_delay_time_,
               std::bind(&SdlPlaybackPanel::delay_hide_timer_handler, this));
    hide_delay_time_ = 0;
  }
  return 0;
}

int SdlPlaybackPanel::get_type() { return 0; }

int SdlPlaybackPanel::event_handler(void *event) {
  if (force_hide_) {
    dirty_ = false;
    return 0;
  }
  SDL_Event *e = (SDL_Event *)event;
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    long long temp_time = -1;
    // check if mouse down point is outside of progress bar, then play or pause
    PLAYBACK_STATE_E pre_state = state_;
    if (!point_in_rect(e->button.x, e->button.y, &(bar_->get_bar_rect()))) {
      if (state_ == PLAYBACK_PLAY) {
        state_ = PLAYBACK_PAUSE;
      } else if (state_ == PLAYBACK_PAUSE) {
        state_ = PLAYBACK_PLAY;
      } else if (state_ == PLAYBACK_EOS) {
        state_ = PLAYBACK_PLAY;
        seek_time_ = 0;
        set_current_time(seek_time_);
        temp_time = seek_time_;
        bar_->set_fraction(0.0);
        bar_->set_run(true);
      }
    } else if (point_in_rect(e->button.x, e->button.y,
                             &(bar_->get_bar_rect()))) {
      int pos = e->button.x - bar_->get_bar_rect().x;
      if (pos < 0) pos = 0;
      seek_time_ = pos / ((float)(bar_->get_bar_rect().w)) * duration_;
      set_current_time(seek_time_);
      state_ = PLAYBACK_SEEK;
      temp_time = seek_time_;
    }
    set_show(true, DELAY_HIDE_TIME);
    if (action_cb_) {
      action a;
      a.state = state_;
      a.seek_time = temp_time;
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
    if (duration_ == current_time_) {
      set_show(true, DELAY_HIDE_TIME);
      state_ = PLAYBACK_EOS;
      if (action_cb_) {
        action a;
        a.state = state_;
        a.seek_time = 0;
        action_cb_(user_data_, &a);
      }
    }
    return 1;
  } else if (e->type == SDL_USER_EVENT_EOS_UPDATE) {
    state_ = PLAYBACK_EOS;
    set_show(true, DELAY_HIDE_TIME);
    if (action_cb_) {
      action a;
      a.state = state_;
      a.seek_time = 0;
      action_cb_(user_data_, &a);
    }
    return 1;
  } else if (e->type == SDL_MOUSEMOTION) {
    if (dirty_) return 0;
    set_show(true, DELAY_HIDE_TIME);
    SDL_Event event;
    event.type = SDL_USER_EVENT_REFRESH;
    event.user.data1 = NULL;
    event.user.data2 = NULL;
    SDL_PushEvent(&event);
    return 1;
  }
  return 0;
}

int SdlPlaybackPanel::set_event_resp_area(int x, int y, int w, int h) {
  return 0;
}

void *SdlPlaybackPanel::get_window() { return nullptr; }

void *SdlPlaybackPanel::get_renderer() { return renderer_; }
