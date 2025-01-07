#ifndef __SDL_IMAGE_H__
#define __SDL_IMAGE_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class SdlImage {
 public:
  SdlImage(SDL_Renderer *renderer, const char *filename);
  ~SdlImage();
  int render_image(SDL_Rect *src, int dst_x, int dst_y);
  int render_image(SDL_Rect *src, SDL_Rect *dst);
  int get_width();
  int get_height();
  int update_file(const char *filename);

 private:
  SDL_Texture *texture_;
  SDL_Renderer *renderer_;
  int width_;
  int height_;
};

#endif