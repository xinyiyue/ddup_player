
#include "gui_widget/sdl_impl/sdl_video.h"

#include <iostream>

#include "gui_widget/sdl_impl/sdl_user_event.h"
#include "log/ddup_log.h"

#define TAG "SdlVideo"

using namespace std;

SdlVideo::SdlVideo(const char *name, kiss_array *arr, kiss_window *win,
                   SDL_Renderer *renderer, int x, int y, int w, int h)
    : Video(name) {
  renderer_ = renderer;
  window_ = win;
  dirty_ = false;
  exit_ = false;
  kiss_makerect(&dst_rect_, x, y, w, h);
  fifo_ = fifo_create(3, sizeof(render_buffer_s *));
  if (!fifo_) {
    LOGI(TAG, "%s", "create fifo failed");
  }
  cond_ = SDL_CreateCond();
  mutex_ = SDL_CreateMutex();
}

int SdlVideo::render_thread(void *data) {
  SdlVideo *video = (SdlVideo *)data;
  int sleep_time = 1 / (float)30 * 1000;  // ms
  while (!video->exit_) {
    if (fifo_is_empty(video->fifo_)) {
      render_buffer_s *buff;
      fifo_get(video->fifo_, &buff);
      if (!buff) {
        LOGI(TAG, "%s", "get buffer null from fifo");
        continue;
      }

      if (!video->texture_) {
        video->texture_ = SDL_CreateTexture(video->renderer_, buff->pixel,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            buff->width, buff->height);
        video->width_ = buff->width;
        video->height_ = buff->height;
        kiss_makerect(&video->src_rect_, 0, 0, buff->width, buff->height);
        sleep_time = 1 / (float)buff->frameRate;
      }
      // SDL_UpdateYUVTexture();
      SDL_UpdateTexture(video->texture_, NULL, buff, buff->width);

      SDL_LockMutex(video->mutex_);
      video->dirty_ = true;
      SDL_UnlockMutex(video->mutex_);
      SDL_Event event;
      event.type = SDL_USER_EVENT_VIDEO_UPDATE;
      event.user.data1 = NULL;
      event.user.data2 = NULL;
      SDL_PushEvent(&event);
    }
    SDL_Delay(sleep_time);
  }
  return 0;
}

int SdlVideo::open(const char *url) {
  render_thread_id_ =
      SDL_CreateThread(SdlVideo::render_thread, "render_Thread", this);
  return 0;
}

int SdlVideo::start() { return 0; }

int SdlVideo::stop() { return 0; }

int SdlVideo::seek(long long seek_time) { return 0; }

int SdlVideo::pause_resume(bool pause) { return 0; }

int SdlVideo::close() {
  exit_ = true;
  return 0;
}

int SdlVideo::draw() {
  SDL_RenderCopy(renderer_, texture_, &src_rect_, &dst_rect_);
  SDL_LockMutex(mutex_);
  dirty_ = false;
  SDL_UnlockMutex(mutex_);
  return 0;
}

int SdlVideo::get_type() { return 0; }

int SdlVideo::event_handler(void *event) {
  SDL_Event *e = (SDL_Event *)event;
  if (e->type == SDL_USER_EVENT_VIDEO_UPDATE) {
    LOGI(TAG, "%s",
         "catch event video update event, will update video picture");
    return 1;
  }
  return 0;
}

int SdlVideo::set_event_resp_area(int x, int y, int w, int h) { return 0; }

void *SdlVideo::get_window() { return window_; }

void *SdlVideo::get_renderer() { return renderer_; }
