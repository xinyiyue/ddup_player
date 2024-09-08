#include "gui_widget/sdl_impl/sdl_vscroll_bar.h"

#include "log/ddup_log.h"

#define TAG "SdlVScrollBar"

SdlVScrollBar::SdlVScrollBar(SDL_Renderer *renderer, SDL_Rect *rect) {
  slider_clicked_ = false;
  SDL_Color gray = {235, 235, 235};
  SDL_Color white = {255, 255, 255};
  rect_ = new SdlRect(renderer, *rect, white, 2, gray);
  up_ = new SdlImage(renderer, "ddup_up.png");
  up_rect_ = {rect->x, rect->y, rect->w, rect->w};
  down_ = new SdlImage(renderer, "ddup_down.png");
  down_rect_ = {rect->x, rect->y + rect->h - rect->w, rect->w, rect->w};
  slider_ = new SdlImage(renderer, "ddup_slider.png");
  slider_rect_ = {rect->x, rect->y + rect->w, rect->w, rect->w};
  moveable_rect_ = {rect->x, rect->y + rect->w, rect->w, rect->h - 2 * rect->w};
  step_ = (rect->h - 3 * rect->w) / 10;
  LOGD(TAG, "rect: %d, %d, %d, %d", rect->x, rect->y, rect->w, rect->h);
  LOGD(TAG, "up_rect: %d, %d, %d, %d", up_rect_.x, up_rect_.y, up_rect_.w,
       up_rect_.h);
  LOGD(TAG, "down_rect: %d, %d, %d, %d", down_rect_.x, down_rect_.y,
       down_rect_.w, down_rect_.h);
  LOGD(TAG, "slider_rect: %d, %d, %d, %d", slider_rect_.x, slider_rect_.y,
       slider_rect_.w, slider_rect_.h);
}

SdlVScrollBar::SdlVScrollBar(SDL_Renderer *renderer, SDL_Rect *rect,
                             const char *up_skin, const char *down_skin,
                             const char *slider_skin) {}

SdlVScrollBar::~SdlVScrollBar() {
  delete rect_;
  delete up_;
  delete down_;
  delete slider_;
}

void SdlVScrollBar::update_slider_hight(double percent) {
  slider_rect_.h = (rect_->get_rect().h - up_rect_.h - down_rect_.h) * percent;
  LOGI(TAG, "slider hight: %d", slider_rect_.h);
}

double SdlVScrollBar::get_slider_up_space() {
  double percent = 1.0 * (slider_rect_.y - rect_->get_rect().y - up_rect_.h) /
                   slider_rect_.h;
  LOGI(TAG, "slider up_space: %f", percent);
  return percent;
}

int SdlVScrollBar::event_handler(void *event) {
  SDL_Event *e = (SDL_Event *)event;
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    if (point_in_rect(e->button.x, e->button.y, &up_rect_)) {
      slider_rect_.y -= step_;
    } else if (point_in_rect(e->button.x, e->button.y, &down_rect_)) {
      slider_rect_.y += step_;
    } else if (point_in_rect(e->button.x, e->button.y, &slider_rect_)) {
      slider_clicked_ = true;
    }
  } else if (e->type == SDL_MOUSEBUTTONUP) {
    slider_clicked_ = false;
  } else if (e->type == SDL_MOUSEMOTION) {
    if (point_in_rect(e->motion.x, e->motion.y, &moveable_rect_) &&
        slider_clicked_) {
      slider_rect_.y += e->motion.yrel;
      LOGD(TAG, "motion: %d, %d, %d, %d", e->motion.x, e->motion.y,
           e->motion.xrel, e->motion.yrel);
    }
  }
  int y = rect_->get_rect().y;
  int h = rect_->get_rect().h;
  if (slider_rect_.y > y + h - down_rect_.h - slider_rect_.h) {
    slider_rect_.y = y + h - down_rect_.h - slider_rect_.h;
  }
  if (slider_rect_.y < y + up_rect_.h) {
    slider_rect_.y = y + up_rect_.h;
  }
  return 0;
}

int SdlVScrollBar::render_vscroll() {
  rect_->render_rect();
  rect_->render_edge();
  up_->render_image(nullptr, &up_rect_);
  slider_->render_image(nullptr, &slider_rect_);
  down_->render_image(nullptr, &down_rect_);
  return 0;
}