#ifndef __SDL_BUTTON__
#define __SDL_BUTTON__

#include "gui_widget/base/button.h"
#include "gui_widget/sdl_impl/sdl_image.h"
#include "gui_widget/sdl_impl/sdl_util.h"

class SdlButton : public Button {
 public:
  SdlButton(const char *name, const char *skin, SDL_Renderer *renderer, int x,
            int y, int w, int h);
  SdlButton(const char *name, const char *skin, SDL_Renderer *renderer);
  virtual ~SdlButton() {
    if (image_) delete image_;
  };
  virtual int draw() final;
  virtual bool is_dirty() final;
  virtual int get_type() final;
  virtual void *get_window() final;
  virtual void *get_renderer() final;
  virtual void set_show(bool show, int time_ms) final;
  virtual int event_handler(void *event) final;
  virtual int set_event_resp_area(int x, int y, int w, int h) final;
  void delay_hide_timer_handler();
  SDL_Renderer *renderer_;

 private:
  SdlImage *image_;
  SDL_Rect rect_;
  SDL_Rect resp_rect_;
};
#endif
