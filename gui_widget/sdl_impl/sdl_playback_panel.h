#ifndef __SDL_PLAYBACK_PANEL__
#define __SDL_PLAYBACK_PANEL__

#include "gui_widget/base/progress_bar.h"
#include "gui_widget/base/widget.h"
#include "gui_widget/sdl_impl/sdl_button.h"
#include "gui_widget/sdl_impl/sdl_font.h"
#include "gui_widget/sdl_impl/sdl_progress_bar.h"
#include "gui_widget/sdl_impl/sdl_rect.h"
#include "gui_widget/sdl_impl/sdl_util.h"

typedef struct action {
  PLAYBACK_STATE_E state;
  long long seek_time;
} action;

class SdlPlaybackPanel : public ProgBar {
 public:
  SDL_Renderer *renderer_;
  SdlProgressBar *bar_;

 public:
  SdlPlaybackPanel(const char *name, const SDL_Rect &rect,
                   const char *prog_skin, SDL_Renderer *renderer);
  virtual ~SdlPlaybackPanel() {
    if (bar_) {
      delete bar_;
      bar_ = NULL;
    }
    if (s_rect_) {
      delete s_rect_;
      s_rect_ = NULL;
    }
    if (font_) {
      delete font_;
      font_ = NULL;
    }
  };

  virtual bool is_dirty() final;
  virtual int draw() final;
  virtual int get_type() final;
  virtual int event_handler(void *event) final;
  virtual int set_event_resp_area(int x, int y, int w, int h) final;
  virtual void *get_window() final;
  virtual void *get_renderer() final;

  virtual void set_duration(long long duration) final;
  virtual void set_current_time(long long current) final;
  void delay_hide_timer_handler();
  void force_hide(bool force_hide) { force_hide_ = force_hide; };

 private:
  SdlRect *s_rect_;
  SDL_Rect dur_label_rect_;
  SDL_Rect time_label_rect_;
  SdlFont *font_;
  char time_str_[11];
  char duration_str_[11];
  bool force_hide_;
};

#endif
