#ifndef __SDL_LABE_H__
#define __SDL_LABE_H__
#include <string>

#include "gui_widget/sdl_impl/sdl_font.h"
#include "gui_widget/sdl_impl/sdl_rect.h"
#include "gui_widget/sdl_impl/sdl_util.h"

class SdlLabel {
 public:
  SdlLabel(SDL_Renderer* renderer, SdlFont* font, const char* text, int x,
           int y, bool need_decorate = true);
  SdlLabel(SDL_Renderer* renderer, SdlFont* font, const char* text,
           SDL_Rect* rect, bool need_decorate = true);
  ~SdlLabel();
  void set_attri(SDL_Color* font_color, SDL_Color* bg_color,
                 SDL_Color* edge_color, int edge_width);
  int event_handler(void* event);
  bool is_selected() { return is_selected_; };
  std::string get_text() { return text_; };
  SDL_Rect* get_rect() { return rect_->get_rect_p(); };
  int render_label();

 private:
  SdlFont* font_;
  std::string text_;
  SdlRect* rect_;
  SDL_Color bg_color_;
  SDL_Color hl_bg_color_;
  SDL_Color edge_color_;
  SDL_Color font_color_;
  int edge_width_;
  bool need_decorate_;
  bool is_selected_;
};

#endif