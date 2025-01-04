#ifndef __IMAGE_WIDGET__
#define __IMAGE_WIDGET__
#include <string>

#include "gui_widget/base/widget.h"
#include "gui_widget/sdl_impl/sdl_image.h"

class SdlImgWidget : public Widget {
 public:
  SdlImgWidget(SDL_Renderer *renderer, const char *filename, SDL_Rect *dst);
  ~SdlImgWidget();
  virtual int draw() override;
  virtual int get_type() { return BUTTON_BASE_TYPE; };
  virtual void *get_window() { return nullptr; };
  virtual void *get_renderer() { return nullptr; };
  virtual int event_handler(void *event) override;
  virtual int set_event_resp_area(int x, int y, int w, int h) { return 0; };

 private:
  SdlImage *image_;
  SDL_Rect rect_;
};
#endif