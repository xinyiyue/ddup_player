
#include "gui_widget/sdl_impl/sdl_label.h"

SdlLabel::SdlLabel(SDL_Renderer* renderer, SdlFont* font, const char* text,
                   int x, int y, bool need_decorate)
    : font_(font), text_(text), need_decorate_(need_decorate) {
  SDL_Rect l_rect = font_->get_text_rect(text);
  l_rect.x = x;
  l_rect.y = y;
  rect_ = new SdlRect(renderer, l_rect);
  if (need_decorate_) {
    bg_color_ = rgba_gray;
    edge_color_ = rgba_white;
    edge_width_ = 1;
    rect_->set_attri(&bg_color_, edge_width_, &edge_color_);
  }
  font_color_ = rgba_white;
}

SdlLabel::SdlLabel(SDL_Renderer* renderer, SdlFont* font, const char* text,
                   SDL_Rect* rect, bool need_decorate)
    : font_(font), text_(text), need_decorate_(need_decorate) {
  rect_ = new SdlRect(renderer, *rect);
  if (need_decorate_) {
    bg_color_ = rgba_gray;
    edge_color_ = rgba_white;
    edge_width_ = 1;
    font_color_ = rgba_white;
    rect_->set_attri(&bg_color_, edge_width_, &edge_color_);
  }
  font_color_ = rgba_white;
}

SdlLabel::~SdlLabel() {
  if (rect_) {
    delete rect_;
    rect_ = nullptr;
  }
}

void SdlLabel::set_attri(SDL_Color* font_color, SDL_Color* bg_color,
                         SDL_Color* edge_color, int edge_width) {
  bg_color_ = *bg_color;
  edge_color_ = *edge_color;
  edge_width_ = edge_width;
  font_color_ = *font_color;
  rect_->set_attri(&bg_color_, edge_width_, &edge_color_);
}

int SdlLabel::render_label() {
  if (need_decorate_) {
    rect_->render_rect();
    rect_->render_edge();
    SDL_Rect temp = rect_->get_rect();
    temp.x += edge_width_;
    temp.y += edge_width_;
    temp.w -= edge_width_ * 2;
    temp.h -= edge_width_ * 2;
    font_->render_text(text_.c_str(), temp, font_color_);
  } else {
    SDL_Rect temp = rect_->get_rect();
    font_->render_text(text_.c_str(), temp, font_color_);
  }
  return 0;
}