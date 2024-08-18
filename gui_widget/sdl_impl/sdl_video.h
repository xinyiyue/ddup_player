#ifndef __SDL_VIDEO_H_H__
#define __SDL_VIDEO_H_H__

#include "gui_widget/base/video.h"
#include "gui_widget/sdl_impl/sdl_texture_builder.h"
#include "player/ddup_player.h"
#include "player/event_listener.h"
#include "third_party/FIFO/FIFO.h"

class SdlVideo : public Video, public EventListener, public SdlTextureBuilder {
 public:
  SdlVideo(const char *name, SDL_mutex *renderer_mutex, SDL_Renderer *renderer,
           int x, int y, int w, int h);
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

  virtual void notify_event(int event_type, void *ret) final;
  virtual void notify_error(int error_type) final;

 private:
  SDL_Renderer *renderer_;
  DDupPlayer *player_;
  bool exit_;
  long long real_time_pos_;
  long long last_pos_;
  long long duration_;
};

#endif
