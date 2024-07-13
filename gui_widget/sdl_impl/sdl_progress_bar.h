#ifndef __SDL_PROGRESS_BAR__
#define __SDL_PROGRESS_BAR__

#include "gui_widget/base/progress_bar.h"
#include "gui_widget/base/widget.h"
#include "gui_widget/sdl_impl/sdl_button.h"
#include "third_party/kiss_sdl/kiss_sdl.h"

typedef struct action {
  PLAYBACK_STATE_E state;
  long long seek_time;
} action;

class SdlProgBar : public ProgBar {
 public:
  SDL_Renderer *renderer_;
  kiss_window *window_;
  kiss_progressbar bar_;
  kiss_label time_label_;
  kiss_label dur_label_;

 public:
  SdlProgBar(const char *name, kiss_window *win, SDL_Renderer *renderer);
  virtual ~SdlProgBar(){};

  virtual bool is_dirty() final;
  virtual int draw() final;
  virtual int get_type() final;
  virtual int event_handler(void *event) final;
  virtual int set_event_resp_area(int x, int y, int w, int h) final;
  virtual void *get_window() final;
  virtual void *get_renderer() final;

  virtual void set_duration(long long duration) final;
  virtual void set_current_time(long long current) final;

 private:
  SDL_Rect rect_;
};

#endif
