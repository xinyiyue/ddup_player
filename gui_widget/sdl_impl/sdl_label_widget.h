#ifndef __LABEL_WIDGET__
#define __LABEL_WIDGET__
#include <string>

#include "gui_widget/base/widget.h"
#include "gui_widget/sdl_impl/sdl_label.h"

class SdlLabelWidget : public Widget {
 public:
  SdlLabelWidget(SDL_Renderer* renderer, SdlFont* font, const char* text, int x,
                 int y, bool need_decorate = true);
  SdlLabelWidget(SDL_Renderer* renderer, SdlFont* font, const char* text,
                 SDL_Rect* rect, bool need_decorate = true);
  ~SdlLabelWidget();
  virtual int draw() override;
  virtual int event_handler(void* event) override;
  virtual int get_type() override { return BUTTON_BASE_TYPE; };
  virtual void* get_window() override { return nullptr; };
  virtual void* get_renderer() override { return nullptr; };
  virtual int set_event_resp_area(int x, int y, int w, int h) override {
    return 0;
  };
  SdlLabel* get_label_handle() { return label_; };

 private:
  SdlLabel* label_;
};
#endif