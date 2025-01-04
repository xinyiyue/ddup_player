#ifndef __DIRENT_WINDOW_WIDGET_H_H__
#define __DIRENT_WINDOW_WIDGET_H_H__

#include "gui_widget/base/widget.h"
#include "gui_widget/sdl_impl/sdl_dirent_window.h"

class SdlDirWinWidget : public Widget {
 public:
  SdlDirWinWidget(SDL_Renderer *renderer, SDL_Rect *rect, SdlFont *font);
  ~SdlDirWinWidget();
  virtual int draw() override;
  virtual int get_type() { return BUTTON_BASE_TYPE; };
  virtual void *get_window() { return nullptr; };
  virtual void *get_renderer() { return nullptr; };
  virtual int event_handler(void *event) override;
  virtual int set_event_resp_area(int x, int y, int w, int h) { return 0; };
  SdlDirentWindow *get() { return dir_win_; }

 private:
  SdlDirentWindow *dir_win_;
  SDL_Rect rect_;
  bool is_played_;
};
#endif
