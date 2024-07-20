#ifndef __SDL_VIDEO_H_H__
#define __SDL_VIDEO_H_H__

#include "gui_widget/base/video.h"
#include "gui_widget/sdl_impl/sdl_texture_builder.h"
#include "player/ddup_player.h"
#include "player/event_listener.h"
#include "third_party/FIFO/FIFO.h"
#include "third_party/kiss_sdl/kiss_sdl.h"

class SdlVideo : public Video, public EventListener, public SdlTextureBuilder {
 public:
  SdlVideo(const char *name, kiss_array *arr, kiss_window *win,
           SDL_Renderer *renderer, int x, int y, int w, int h);
  virtual ~SdlVideo();

  virtual int open(const char *url) final;
  virtual int set_speed(float speed) final;
  virtual int stop() final;
  virtual int seek(long long seekTime) final;
  virtual int close() final;

  virtual int draw() final;
  virtual int get_type() final;
  virtual int event_handler(void *event) final;
  virtual int set_event_resp_area(int x, int y, int w, int h) final;
  virtual void *get_window() final;
  virtual void *get_renderer() final;

 private:
  SDL_Renderer *renderer_;
  kiss_window *window_;
  DDupPlayer *player_;
  bool exit_;
};

#endif
