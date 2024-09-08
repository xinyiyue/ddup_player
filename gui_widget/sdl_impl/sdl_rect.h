#ifndef __SDL_RECT_H__
#define __SDL_RECT_H__

#include <SDL2/SDL.h>

class SdlRect {
 public:
  SdlRect(SDL_Renderer *renderer, const SDL_Rect &rect);
  SdlRect(SDL_Renderer *renderer, const SDL_Rect &rect, const SDL_Color &c,
          int edge, const SDL_Color &e);
  ~SdlRect();
  void set_attri(SDL_Color *c, int edge, const SDL_Color *e);
  SDL_Rect &get_rect() { return rect_; };
  SDL_Rect *get_rect_p() { return &rect_; };
  void update_rect(int x, int y, int w, int h) {
    rect_.x = x > 0 ? x : rect_.x;
    rect_.y = y > 0 ? y : rect_.y;
    rect_.w = w > 0 ? w : rect_.w;
    rect_.h = h > 0 ? h : rect_.h;
  };
  int render_rect();
  int render_edge();

 private:
  SDL_Renderer *renderer_;
  SDL_Rect rect_;
  SDL_Color color_;
  SDL_Color edge_color_;
  int edge_;
};

#endif