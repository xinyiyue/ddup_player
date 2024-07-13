#ifndef __SDL_VIDEO_H_H__
#define __SDL_VIDEO_H_H__

#include "gui_widget/base/video.h"
#include "third_party/FIFO/FIFO.h"
#include "third_party/kiss_sdl/kiss_sdl.h"

typedef struct RenderBuffer {
  char *data;
  int len;
  int width;
  int height;
  int pixel;
  int frameRate;
} render_buffer_s;

class SdlVideo : public Video {
 public:
  SdlVideo(const char *name, kiss_array *arr, kiss_window *win,
           SDL_Renderer *renderer, int x, int y, int w, int h);
  virtual ~SdlVideo() {
    SDL_DestroyCond(cond_);
    SDL_DestroyMutex(mutex_);
  };

  virtual int open(const char *url) final;
  virtual int start() final;
  virtual int stop() final;
  virtual int seek(long long seekTime) final;
  virtual int pause_resume(bool pause) final;
  virtual int close() final;

  virtual int draw() final;
  virtual int get_type() final;
  virtual int event_handler(void *event) final;
  virtual int set_event_resp_area(int x, int y, int w, int h) final;
  virtual void *get_window() final;
  virtual void *get_renderer() final;

  static int render_thread(void *data);

 public:
  SDL_Renderer *renderer_;
  kiss_window *window_;
  SDL_Rect dst_rect_;
  SDL_Rect src_rect_;
  SDL_Texture *texture_;
  int width_;
  int height_;
  SDL_Thread *render_thread_id_;
  fifo_t fifo_;
  SDL_cond *cond_;
  SDL_mutex *mutex_;
  bool exit_;
};

#endif
