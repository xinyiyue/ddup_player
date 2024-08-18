#ifndef __SDL_FONTH_H__
#define __SDL_FONTH_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
class SdlFont {
 public:
  SdlFont(SDL_Renderer *renderer_, const char *font_path, int font_size = 15);
  ~SdlFont();
  int render_text(const char *text, int x, int y, const SDL_Color &color);
  int render_text(const char *text, const SDL_Rect &dst_rect,
                  const SDL_Color &color);

 private:
  SDL_Renderer *renderer_;
  TTF_Font *font_;
  int height_;
  int spacing_;
  int line_height_;
  int advance_;
  int ascent_;
  double base_spacing_;
};

#endif