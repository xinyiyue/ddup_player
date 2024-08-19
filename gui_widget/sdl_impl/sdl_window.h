#ifndef __KISS_WINDOW__
#define __KISS_WINDOW__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <pthread.h>

#include "gui_widget/base/window.h"

class SdlWindow : public Window {
 public:
  SdlWindow(const char *name, int w, int h);
  ~SdlWindow();
  virtual int create() final;
  virtual int update() final;
  virtual int show() final;
  virtual int event_handler(void *event) final;
  SDL_Renderer *renderer_;
  SDL_Window *window_;
  SDL_mutex *renderer_mutex_;

 private:
  bool exit_;
};
#endif
