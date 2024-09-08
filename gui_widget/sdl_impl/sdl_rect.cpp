#include "gui_widget/sdl_impl/sdl_rect.h"

#include "log/ddup_log.h"

#define TAG "SdlRect"

SdlRect::SdlRect(SDL_Renderer *renderer, const SDL_Rect &rect) {
  renderer_ = renderer;
  rect_ = rect;
}

SdlRect::SdlRect(SDL_Renderer *renderer, const SDL_Rect &rect,
                 const SDL_Color &c, int edge, const SDL_Color &e) {
  renderer_ = renderer;
  color_ = c;
  edge_color_ = e;
  rect_ = rect;
  edge_ = edge;
}

SdlRect::~SdlRect() {}
void SdlRect::set_attri(SDL_Color *c, int edge, const SDL_Color *e) {
  color_ = *c;
  edge_ = edge;
  edge_color_ = *e;
}

int SdlRect::render_rect() {
  LOGD(TAG, "rect:%d,%d,%d,%d, color:%d,%d,%d,%d", rect_.x, rect_.y, rect_.w,
       rect_.h, color_.r, color_.g, color_.b, color_.a);
  SDL_SetRenderDrawColor(renderer_, color_.r, color_.g, color_.b, color_.a);
  SDL_RenderFillRect(renderer_, &rect_);
  return 0;
}

int SdlRect::render_edge() {
  LOGD(TAG, "render_edge:%d,%d,%d,%d", edge_color_.r, edge_color_.g,
       edge_color_.b, edge_color_.a);
  SDL_SetRenderDrawColor(renderer_, edge_color_.r, edge_color_.g, edge_color_.b,
                         edge_color_.a);

  SDL_Rect outlinerect;
  for (int i = 0; i < edge_; i++) {
    outlinerect = {rect_.x + i, rect_.y + i, rect_.w - 2 * i, rect_.h - 2 * i};
    LOGD(TAG, "outlinerect:%d,%d,%d,%d", outlinerect.x, outlinerect.y,
         outlinerect.w, outlinerect.h);
    SDL_RenderDrawRect(renderer_, &outlinerect);
  }
  return 0;
}