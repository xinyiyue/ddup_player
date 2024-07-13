#ifndef __KISS_WINDOW__
#define __KISS_WINDOW__

#include <pthread.h>

#include "gui_widget/base/window.h"
#include "third_party/kiss_sdl/kiss_sdl.h"

class SdlWindow : public Window {
 public:
  SdlWindow(const char *name, int w, int h);
  ~SdlWindow();
  virtual int create() final;
  virtual int update() final;
  virtual int show() final;
  virtual int event_handler(void *event) final;
  SDL_Renderer *renderer_;
  kiss_window window_;
  kiss_array array_;

 private:
  bool exit_;
};
#endif
