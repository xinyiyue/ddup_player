#ifndef __SDL_IMAGE_VIEW_PANEL__
#define __SDL_IMAGE_VIEW_PANEL__

#include "gui_widget/base/widget.h"
#include "gui_widget/sdl_impl/sdl_button.h"

class SdlImageViewPanel : public Widget {
 public:
  SDL_Renderer *renderer_;

  SdlImageViewPanel(const char *name, const SDL_Rect &rect);
  virtual ~SdlImageViewPanel(){

  };

  virtual bool is_dirty() final;
  virtual int draw() final;
  virtual int get_type() final;
  virtual int event_handler(void *event) final;
  virtual int set_event_resp_area(int x, int y, int w, int h) final;
  virtual void *get_window() final;
  virtual void *get_renderer() final;

  void set_buttons(SdlButton *prev_button, SdlButton *next_button) {
    prev_button_ = prev_button;
    next_button_ = next_button;
  }

  SdlButton *prev_button_;
  SdlButton *next_button_;

  SDL_Rect rect_;
};

#endif
