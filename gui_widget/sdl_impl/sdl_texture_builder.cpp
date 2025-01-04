
#include "gui_widget/sdl_impl/sdl_texture_builder.h"

#include <SDL2/SDL.h>

#include "gui_widget/sdl_impl/sdl_user_event.h"
#include "log/ddup_log.h"
#include "player/util.h"

#define TAG "SdlTextureBuilder"
static const char *print_sdl_pixel_name(SDL_PixelFormatEnum pixel) {
  Uint32 format = pixel;
  SDL_PixelFormat pixelFormat;
  SDL_zero(pixelFormat);
  pixelFormat.format = format;

  Uint32 masks[4];
  SDL_PixelFormatEnumToMasks(format, (int *)&pixelFormat.BitsPerPixel, masks,
                             masks + 1, masks + 2, masks + 3);

  const char *formatName = SDL_GetPixelFormatName(format);
  LOGD(TAG, "SDL_PixelFormatEnum:%d Format name: %s\n", pixel, formatName);
  return formatName;
}

SDL_PixelFormatEnum SdlTextureBuilder::map_to_sdl_pixel_format(
    pixel_format_t format) {
  SDL_PixelFormatEnum pixel;
  switch (format) {
    case PIXELFORMAT_ARGB8888:
      pixel = SDL_PIXELFORMAT_ARGB8888;
      break;
    case PIXELFORMAT_BGRA8888:
      pixel = SDL_PIXELFORMAT_BGRA8888;
      break;
    case PIXELFORMAT_IYUV:
      pixel = SDL_PIXELFORMAT_IYUV;
      break;
    case PIXELFORMAT_YUY2:
      pixel = SDL_PIXELFORMAT_YUY2;
      break;
    case PIXELFORMAT_UYVY:
      pixel = SDL_PIXELFORMAT_UYVY;
      break;
    default:
      pixel = SDL_PIXELFORMAT_UNKNOWN;
      break;
  }
  LOGD(TAG, "map ddup fromat:%d sdl format name: %s\n", format,
       print_sdl_pixel_name(pixel));
  return pixel;
}

SdlTextureBuilder::SdlTextureBuilder(const char *name,
                                     SDL_mutex *renderer_mutex,
                                     SDL_Renderer *renderer, int x, int y,
                                     int w, int h)
    : TextureBuilder(name) {
  renderer_ = renderer;
  mutex_ = SDL_CreateMutex();
  dst_rect_ = {x, y, w, h};
  win_width_ = w;
  win_height_ = h;
  texture_ = nullptr;
  renderer_mutex_ = renderer_mutex;
}

SdlTextureBuilder::~SdlTextureBuilder() {
  if (mutex_) {
    SDL_DestroyMutex(mutex_);
  }
  if (texture_) {
    LOGI(TAG, "%s", "destory texture in ~SdlTextureBuilder");
    SDL_DestroyTexture(texture_);
  }
}

int SdlTextureBuilder::get_supported_format(video_format_s *format) {
  format->width = win_width_;
  format->height = win_height_;
  format->pixel[0] = PIXELFORMAT_IYUV;
  // format->pixel[0] = PIXELFORMAT_ARGB8888;
  LOGI(TAG, "get supported format,w:%d, h:%d, pixel:%s", win_width_,
       win_height_,
       print_sdl_pixel_name(map_to_sdl_pixel_format(format->pixel[0])));

  return 0;
}

int SdlTextureBuilder::set_negotiated_format(video_format_s *format) {
  width_ = format->width;
  height_ = format->width;
  pixel_format_ = map_to_sdl_pixel_format(format->pixel[0]);
  return 0;
}

int SdlTextureBuilder::build_texture(render_buffer_s *buff) {
  SDL_PixelFormatEnum pixel = map_to_sdl_pixel_format(buff->pixel);
  if (!texture_ || pixel != pixel_format_) {
    if (!texture_) {
      texture_ =
          SDL_CreateTexture(renderer_, pixel, SDL_TEXTUREACCESS_STREAMING,
                            buff->width, buff->height);
      LOGI(TAG, "create texture,w:%d, h:%d, pixel:%s", buff->width,
           buff->height, print_sdl_pixel_name(pixel));
    } else {
      LOGI(TAG, "format change, recreate texture,w:%d, h:%d, pixel:%s",
           buff->width, buff->height, print_sdl_pixel_name(pixel));
      SDL_DestroyTexture(texture_);
      texture_ =
          SDL_CreateTexture(renderer_, pixel, SDL_TEXTUREACCESS_STREAMING,
                            buff->width, buff->height);
    }
    width_ = buff->width;
    height_ = buff->height;
    pixel_format_ = pixel;
    src_rect_ = {0, 0, width_, height_};
  }
  AutoLock lock(renderer_mutex_);
  if (pixel_format_ == SDL_PIXELFORMAT_IYUV) {
    LOGD(TAG, "SDL_UpdateYUVTexture pixel:%s", print_sdl_pixel_name(pixel));
    SDL_UpdateYUVTexture(texture_, NULL, (const Uint8 *)buff->data[0],
                         buff->len[0], (const Uint8 *)buff->data[1],
                         buff->len[1], (const Uint8 *)buff->data[2],
                         buff->len[2]);
  } else {
    LOGD(TAG, "SDL_UpdateYUVTexture pixel:%s", print_sdl_pixel_name(pixel));
    SDL_UpdateTexture(texture_, NULL, buff->data[0], width_ * 4);
  }
  SDL_Event event;
  event.type = SDL_USER_EVENT_VIDEO_UPDATE;
  event.user.data1 = NULL;
  event.user.data2 = NULL;
  SDL_PushEvent(&event);
  return 0;
}

int SdlTextureBuilder::render_texture() {
  SDL_RenderCopy(renderer_, texture_, &src_rect_, &dst_rect_);
  return 0;
}
