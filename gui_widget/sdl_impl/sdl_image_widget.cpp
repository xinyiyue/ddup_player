
#include "gui_widget/sdl_impl/sdl_image_widget.h"

#include "gui_widget/sdl_impl/sdl_user_event.h"

SdlImgWidget::SdlImgWidget(SDL_Renderer *renderer, const char *filename,
                           SDL_Rect *dst)
    : Widget("image") {
  image_ = new SdlImage(renderer, filename);
  rect_ = *dst;
  dirty_ = true;
}

SdlImgWidget::~SdlImgWidget() { delete image_; }

int SdlImgWidget::event_handler(void *event) {
  SDL_Event *e = (SDL_Event *)event;
  if (action_cb_ && e->type == SDL_USER_EVENT_IMAGE_HIDE) {
    action_cb_(user_data_, event);
    return 1;
  }
  return 0;
}

int SdlImgWidget::draw() { return image_->render_image(nullptr, &rect_); }