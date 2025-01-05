#include "gui_widget/sdl_impl/sdl_font.h"

#include "log/ddup_log.h"

#define TAG "SdlFont"

SdlFont::SdlFont(SDL_Renderer *renderer, const char *font_path, int font_size)
    : renderer_(renderer), base_spacing_(0.5) {
  if (!(font_ = TTF_OpenFont(font_path, font_size))) {
    LOGE(TAG, "Cannot load font %s", font_path);
  }
  height_ = TTF_FontHeight(font_);
  spacing_ = (int)(base_spacing_ * height_);
  line_height_ = height_ + spacing_;
  ascent_ = TTF_FontAscent(font_);
  TTF_GlyphMetrics(font_, 'W', NULL, NULL, NULL, NULL, &advance_);
  LOGI(TAG, "font height %d, spaceing:%d, line_height:%d, ascent:%d", height_,
       spacing_, line_height_, ascent_);
}

SdlFont::~SdlFont() {
  if (font_) {
    LOGI(TAG, "%s", "close font");
    TTF_CloseFont(font_);
  }
  font_ = nullptr;
}

SDL_Rect SdlFont::get_text_rect(const char *text) {
  int w, h;
  TTF_SizeUTF8(font_, text, &w, &h);
  SDL_Rect rect = {0};
  rect.x = 0;
  rect.y = 0;
  rect.w = w;
  rect.h = h;
  LOGD(TAG, "text:%s, rect(%d, %d, %d, %d)", text, rect.x, rect.y, rect.w,
       rect.h);
  return rect;
}

int SdlFont::render_text(const char *text, int x, int y,
                         const SDL_Color &color) {
  SDL_Surface *surface = TTF_RenderUTF8_Blended(font_, text, color);
  if (!surface) {
    LOGE(TAG, "Cannot render text %s", text);
    return -1;
  }
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer_, surface);
  SDL_Rect text_rect;
  text_rect.x = x;
  text_rect.y = y;
  SDL_QueryTexture(texture, NULL, NULL, &text_rect.w, &text_rect.h);
  LOGD(TAG, "text:%s, rect(%d, %d, %d, %d)", text, text_rect.x, text_rect.y,
       text_rect.w, text_rect.h);
  SDL_RenderCopy(renderer_, texture, nullptr, &text_rect);
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
  return 0;
}

int SdlFont::render_text(const char *text, const SDL_Rect &dst_rect,
                         const SDL_Color &color) {
  SDL_Surface *surface = TTF_RenderUTF8_Blended(font_, text, color);
  if (!surface) {
    LOGE(TAG, "Cannot render text %s", text);
    return -1;
  }
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer_, surface);
  SDL_RenderCopy(renderer_, texture, nullptr, &dst_rect);
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
  return 0;
}
