#include "gui_widget/sdl_impl/sdl_image.h"

#include <string>

#include "log/ddup_log.h"

#define TAG "SdlImage"
SdlImage::SdlImage(SDL_Renderer *renderer, const char *filename)
    : width_(0), height_(0), renderer_(renderer) {
  texture_ = IMG_LoadTexture(renderer_, filename);
  if (texture_ == nullptr) {
    LOGE(TAG, "Failed to create texture from image:%s", filename);
  } else {
    SDL_QueryTexture(texture_, nullptr, nullptr, &width_, &height_);
    LOGE(TAG, "image:%s, w:%d, h:%d", filename, width_, height_);
  }
}

SdlImage::~SdlImage() { SDL_DestroyTexture(texture_); }
int SdlImage::get_width() { return width_; }

int SdlImage::get_height() { return height_; }

int SdlImage::render_image(SDL_Rect *src, int dst_x, int dst_y) {
  SDL_Rect dst = {dst_x, dst_y, width_, height_};
  SDL_RenderCopy(renderer_, texture_, src, &dst);
  return 0;
}
int SdlImage::render_image(SDL_Rect *src, SDL_Rect *dst) {
  if (dst == nullptr) {
    return -1;
  }
  SDL_RenderCopy(renderer_, texture_, src, dst);
  return 0;
}