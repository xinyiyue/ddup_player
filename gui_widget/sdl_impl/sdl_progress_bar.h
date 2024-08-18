#ifndef __SDL_PROGRESS_BAR_BASE_H__
#define __SDL_PROGRESS_BAR_BASE_H__

#include <string>

#include "gui_widget/sdl_impl/sdl_image.h"
#include "gui_widget/sdl_impl/sdl_rect.h"

class SdlProgressBar {
 public:
  SdlProgressBar(SDL_Renderer *renderer, const char *name,
                 const char *sink_path);
  virtual ~SdlProgressBar();
  void set_step(double step);
  void set_fraction(double fraction);
  void set_run(bool run);
  bool get_run();
  SDL_Rect &get_bar_rect() {
    if (fill_rect_) return fill_rect_->get_rect();
  };
  int set_bar_rect(const SDL_Rect &rect, const SDL_Color &fill_color, int edge,
                   const SDL_Color &edge_color);
  int render_bar();

 protected:
  SDL_Renderer *renderer_;
  SdlImage *image_;
  SdlRect *fill_rect_;
  SDL_Rect bar_rect_;
  int width_;
  int edged_width_;
  int space_;
  double fraction_;
  double step_;
  SDL_Color bg_color_;
  unsigned int last_tick_;
  bool run_;
  std::string name_;
};

#endif
