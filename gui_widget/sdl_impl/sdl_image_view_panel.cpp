#include "gui_widget/sdl_impl/sdl_image_view_panel.h"

#include <iostream>

#include "gui_widget/sdl_impl/sdl_user_event.h"
#include "log/ddup_log.h"

#define TAG "SdlImageViewPanel"

#define DELAY_HIDE_TIME 5000

SdlImageViewPanel::SdlImageViewPanel(const char *name, const SDL_Rect &rect)
    : Widget(name) {
  rect_ = rect;
}

bool SdlImageViewPanel::is_dirty() { return dirty_; }

int SdlImageViewPanel::draw() { return 0; }

int SdlImageViewPanel::get_type() { return 0; }

int SdlImageViewPanel::event_handler(void *event) {
  SDL_Event *e = (SDL_Event *)event;
  if (e->type == SDL_MOUSEMOTION) {
    prev_button_->set_show(true, DELAY_HIDE_TIME);
    next_button_->set_show(true, DELAY_HIDE_TIME);
    SDL_Event event;
    event.type = SDL_USER_EVENT_REFRESH;
    event.user.data1 = NULL;
    event.user.data2 = NULL;
    SDL_PushEvent(&event);
    return 1;
  }
  return 0;
}

int SdlImageViewPanel::set_event_resp_area(int x, int y, int w, int h) {
  return 0;
}

void *SdlImageViewPanel::get_window() { return nullptr; }

void *SdlImageViewPanel::get_renderer() { return renderer_; }
