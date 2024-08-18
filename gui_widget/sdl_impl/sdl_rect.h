#ifndef __SDL_RECT_H__
#define __SDL_RECT_H__

#include <SDL2/SDL.h>

class SdlRect {
 public:
  SdlRect(SDL_Renderer *renderer, const SDL_Rect &rect, const SDL_Color &c,
          int edge, const SDL_Color &e);
  ~SdlRect();
  SDL_Rect &get_rect() { return rect_; };
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