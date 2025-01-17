#ifndef __SDL_AUDIO_H_H__
#define __SDL_AUDIO_H_H__

#include "gui_widget/sdl_impl/sdl_gif.h"
#include "player/ddup_player.h"
#include "player/event_listener.h"
#include "third_party/FIFO/FIFO.h"

class SdlAudio : public Widget, public EventListener {
 public:
  SdlAudio(const char *name, const char *gif, SDL_mutex *renderer_mutex,
           SDL_Renderer *renderer, int x, int y, int w, int h);
  SdlAudio(const char *name, SdlGif *gif);
  virtual ~SdlAudio();

  int open(const char *url);
  int set_speed(float speed);
  int stop();
  int seek(long long seekTime);
  int play_next(const char *url);
  int close();

  virtual int draw() final;
  virtual int get_type() final;
  virtual int event_handler(void *event) final;
  virtual int set_event_resp_area(int x, int y, int w, int h) final;
  virtual void *get_window() final;
  virtual void *get_renderer() final;

  virtual void notify_event(int event_type, void *ret) final;
  virtual void notify_error(int error_type) final;

  ddup_state_t get_state() { return player_->get_state(); };

 private:
  SDL_Renderer *renderer_;
  DDupPlayer *player_;
  SdlGif *gif_;
  long long real_time_pos_;
  long long last_pos_;
  long long duration_;
};

#endif