
#include "gui_widget/sdl_impl/sdl_texture_builder.h"

#include <SDL2/SDL.h>

#include "gui_widget/sdl_impl/sdl_user_event.h"
#include "log/ddup_log.h"
#include "player/util.h"
#include "third_party/kiss_sdl/kiss_sdl.h"

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
      // pixel = SDL_PIXELFORMAT_ARGB8888;
      pixel = SDL_PIXELFORMAT_BGRA8888;
      break;
    case PIXELFORMAT_BGRA8888:
      pixel = SDL_PIXELFORMAT_BGRA8888;
      break;
    case PIXELFORMAT_NV12:
      pixel = SDL_PIXELFORMAT_NV12;
      break;
    case PIXELFORMAT_NV21:
      pixel = SDL_PIXELFORMAT_NV21;
      break;
    default:
      pixel = SDL_PIXELFORMAT_UNKNOWN;
      break;
  }
  LOGD(TAG, "map ddup fromat:%d sdl format name: %s\n", format,
       print_sdl_pixel_name(pixel));
  return pixel;
}

SdlTextureBuilder::SdlTextureBuilder(const char *name, SDL_Renderer *renderer,
                                     int x, int y, int w, int h)
    : TextureBuilder(name) {
  renderer_ = renderer;
  mutex_ = SDL_CreateMutex();
  kiss_makerect(&dst_rect_, x, y, w, h);
  win_width_ = w;
  win_height_ = h;
  texture_ = nullptr;
}

SdlTextureBuilder::~SdlTextureBuilder() {
  if (mutex_) {
    SDL_DestroyMutex(mutex_);
  }
}

int SdlTextureBuilder::get_supported_format(video_format_s *format) {
  format->width = win_width_;
  format->height = win_height_;
  format->pixel[0] = PIXELFORMAT_ARGB8888;
  return 0;
}

int SdlTextureBuilder::set_negotiated_format(video_format_s *format) {
  width_ = format->width;
  height_ = format->width;
  pixel_format_ = map_to_sdl_pixel_format(format->pixel[0]);
  return 0;
}

int SdlTextureBuilder::build_texture(render_buffer_s *buff) {
  AutoLock lock(mutex_);
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
    kiss_makerect(&src_rect_, 0, 0, width_, height_);
  }
  SDL_UpdateTexture(texture_, NULL, buff->data, buff->width * 4);
  SDL_Event event;
  event.type = SDL_USER_EVENT_VIDEO_UPDATE;
  event.user.data1 = NULL;
  event.user.data2 = NULL;
  SDL_PushEvent(&event);
  return 0;
}

int SdlTextureBuilder::render_texture() {
  AutoLock lock(mutex_);
  SDL_RenderCopy(renderer_, texture_, &src_rect_, &dst_rect_);
  return 0;
}
