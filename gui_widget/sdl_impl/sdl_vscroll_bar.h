#ifndef __SDL_VSCROLL_BAR_H__
#define __SDL_VSCROLL_BAR_H__

#include "gui_widget/sdl_impl/sdl_image.h"
#include "gui_widget/sdl_impl/sdl_rect.h"
#include "gui_widget/sdl_impl/sdl_util.h"

class SdlVScrollBar {
 public:
  SdlVScrollBar(SDL_Renderer *renderer, SDL_Rect *rect);
  SdlVScrollBar(SDL_Renderer *renderer, SDL_Rect *rect, const char *up_skin,
                const char *down_skin, const char *slider_skin);
  ~SdlVScrollBar();
  void update_slider_hight(double percent);
  double get_slider_up_space();
  int event_handler(void *event);
  int render_vscroll();
  SdlImage *up_;
  SdlImage *down_;
  SdlImage *slider_;
  SdlRect *rect_;
  double step_;
  SDL_Rect up_rect_;
  SDL_Rect down_rect_;
  SDL_Rect slider_rect_;
  SDL_Rect moveable_rect_;
  bool slider_clicked_;
};

#endif