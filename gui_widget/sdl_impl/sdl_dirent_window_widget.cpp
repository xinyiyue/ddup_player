
#include "gui_widget/sdl_impl/sdl_dirent_window_widget.h"

#include "gui_widget/sdl_impl/sdl_user_event.h"

#define TAG "SdlDirWinWidget"

SdlDirWinWidget::SdlDirWinWidget(SDL_Renderer *renderer, SDL_Rect *rect,
                                 SdlFont *font)
    : Widget("dirwin") {
  rect_ = *rect;
  dir_win_ = new SdlDirentWindow(renderer, rect_, font);
  is_played_ = false;
  dirty_ = false;
}

SdlDirWinWidget::~SdlDirWinWidget() { delete dir_win_; }

int SdlDirWinWidget::event_handler(void *event) {
  bool cached = false;
  dirty_ = false;
  SDL_Event *e = (SDL_Event *)event;
  if (e->type == SDL_USER_EVENT_REFRESH && is_played_ == false) {
    LOGI(TAG, "%s", "catch event 100, force update dirent window");
    dirty_ = true;
    return 1;
  }
  if (is_played_ == false) cached = dir_win_->event_handler(event);
  if (cached && action_cb_ && is_played_ == false) {
    std::string url = dir_win_->get_play_url();
    if (url.size() > 0) {
      action_cb_(user_data_, &url);
      is_played_ = true;
    }
  }

  if (cached && is_played_ == false) {
    dirty_ = true;
  }
  LOGD(TAG, "SdlDirWinWidget:%p, catch event:%d, dirty:%d, is_played_:%d", this,
       cached, dirty_, is_played_);
  return cached;
}

int SdlDirWinWidget::draw() {
  dir_win_->render_dirent_window();
  dirty_ = false;
  return 0;
}
