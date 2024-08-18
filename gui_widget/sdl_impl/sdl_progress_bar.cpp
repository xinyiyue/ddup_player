#include "gui_widget/sdl_impl/sdl_progress_bar.h"

#include "log/ddup_log.h"

#define TAG "SdlProgressBar"

SdlProgressBar::SdlProgressBar(SDL_Renderer *renderer, const char *name,
                               const char *sink_path)
    : renderer_(renderer), name_(name) {
  image_ = new SdlImage(renderer_, sink_path);
  bar_rect_ = {0, 0, 0, 0};
  width_ = 0;
  edged_width_ = 0;
  fraction_ = 0.;
  step_ = 0.01;
  last_tick_ = 0;
  space_ = 5;
  run_ = false;
}

SdlProgressBar::~SdlProgressBar() {
  if (image_) delete image_;
}

int SdlProgressBar::set_bar_rect(const SDL_Rect &rect,
                                 const SDL_Color &fill_color, int edge,
                                 const SDL_Color &edge_color) {
  LOGW(TAG, "bar height:%d,image height:%d", rect.h, image_->get_height());
  fill_rect_ = new SdlRect(renderer_, rect, fill_color, edge, edge_color);
  if (space_ > rect.h / 10) {
    space_ = rect.h / 10;
  }
  edged_width_ = edge;
  width_ = rect.w - edged_width_ * 2;
  bar_rect_ = rect;
  bar_rect_.x = rect.x + edged_width_;
  bar_rect_.y = rect.y + edged_width_ + space_;
  bar_rect_.w = 0;
  bar_rect_.h = (rect.h - edged_width_ * 2 - space_ * 2);

  return 0;
}

void SdlProgressBar::set_run(bool run) { run_ = run; }

bool SdlProgressBar::get_run() { return run_; }

void SdlProgressBar::set_step(double step) { step_ = step; }

void SdlProgressBar::set_fraction(double fraction) { fraction_ = fraction; }

int SdlProgressBar::render_bar() {
  if (run_) {
    fraction_ += step_;
    if (fraction_ > 0.999999) {
      // run_ = 0;
      fraction_ = 1.;
    }
    if (fraction_ < 1.0) {
      bar_rect_.w = (int)(width_ * fraction_ + 0.05);
    } else {
      bar_rect_.w = width_;
    }
    fill_rect_->render_rect();
    fill_rect_->render_edge();
    image_->render_image(nullptr, &bar_rect_);
    last_tick_ = SDL_GetTicks();
  }
  return 0;
}