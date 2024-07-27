#ifndef __SDL_BUTTON__
#define __SDL_BUTTON__

#include "gui_widget/base/button.h"
#include "third_party/kiss_sdl/kiss_sdl.h"

class SdlButton : public Button {
 public:
  SdlButton(const char *name, const char *skin, kiss_array *arr,
            kiss_window *win, SDL_Renderer *renderer, int x, int y, int w,
            int h);
  SdlButton(const char *name, const char *skin, kiss_array *arr,
            kiss_window *win, SDL_Renderer *renderer);
  virtual ~SdlButton(){};
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
  kiss_window *window_;

 private:
  kiss_image image_;
  SDL_Rect rect_;
  SDL_Rect resp_rect_;
};
#endif
