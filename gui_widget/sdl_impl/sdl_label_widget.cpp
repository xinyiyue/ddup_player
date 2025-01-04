#include "gui_widget/sdl_impl/sdl_label_widget.h"

#include "gui_widget/sdl_impl/sdl_user_event.h"

#define TAG "SdlLabelWidget"

SdlLabelWidget::SdlLabelWidget(SDL_Renderer* renderer, SdlFont* font,
                               const char* text, int x, int y,
                               bool need_decorate)
    : Widget("label") {
  label_ = new SdlLabel(renderer, font, text, x, y, need_decorate);
  dirty_ = true;
}

SdlLabelWidget::SdlLabelWidget(SDL_Renderer* renderer, SdlFont* font,
                               const char* text, SDL_Rect* rect,
                               bool need_decorate)
    : Widget("label") {
  label_ = new SdlLabel(renderer, font, text, rect, need_decorate);
  dirty_ = true;
}

SdlLabelWidget::~SdlLabelWidget() { delete label_; }

int SdlLabelWidget::event_handler(void* event) {
  label_->event_handler(event);
  if (action_cb_ && label_->is_selected() && dirty_) {
    action_cb_(user_data_, nullptr);
    SDL_Event event;
    event.type = SDL_USER_EVENT_REFRESH;
    event.user.data1 = NULL;
    event.user.data2 = NULL;
    SDL_PushEvent(&event);
  }
  return 0;
}

int SdlLabelWidget::draw() { return label_->render_label(); }