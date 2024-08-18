#ifndef __SDL_TEXTURE_BUILDER_H__
#define __SDL_TEXTURE_BUILDER_H__
#include <SDL2/SDL.h>

#include "gui_widget/base/texture_builder.h"
#include "player/buffer.h"

class SdlTextureBuilder : public TextureBuilder {
 public:
  SdlTextureBuilder(const char *name, SDL_mutex *renderer_mutex,
                    SDL_Renderer *renderer, int x, int y, int w, int h);
  ~SdlTextureBuilder();

  SDL_PixelFormatEnum map_to_sdl_pixel_format(pixel_format_t format);
  virtual int get_supported_format(video_format_s *format) override;
  virtual int set_negotiated_format(video_format_s *format) override;
  virtual int build_texture(render_buffer_s *buff) override;
  virtual int render_texture() override;

  int width_;
  int height_;
  int win_width_;
  int win_height_;
  SDL_PixelFormatEnum pixel_format_;
  SDL_Texture *texture_;
  SDL_mutex *mutex_;
  SDL_Rect dst_rect_;
  SDL_Rect src_rect_;

 private:
  SDL_Renderer *renderer_;
  SDL_mutex *renderer_mutex_;
};

#endif
