
#include "gui_widget/sdl_impl/sdl_image_widget.h"

#include "gui_widget/sdl_impl/sdl_user_event.h"

SdlImgWidget::SdlImgWidget(SDL_Renderer *renderer, const char *filename,
                           SDL_Rect *dst, bool full_screen)
    : Widget("image") {
  image_ = new SdlImage(renderer, filename);
  rect_ = *dst;
  dirty_ = true;
  full_screen_ = full_screen;
}

SdlImgWidget::SdlImgWidget(const char *name, SDL_Renderer *renderer,
                           const char *filename, SDL_Rect *dst,
                           bool full_screen)
    : Widget(name) {
  image_ = new SdlImage(renderer, filename);
  rect_ = *dst;
  dirty_ = true;
  full_screen_ = full_screen;
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

int SdlImgWidget::update_file(const char *filename) {
  int ret = image_->update_file(filename);
  if (ret) {
    SDL_Event event;
    event.type = SDL_USER_EVENT_REFRESH;
    event.user.data1 = NULL;
    event.user.data2 = NULL;
    SDL_PushEvent(&event);
  }
  return ret;
}

int SdlImgWidget::draw() {
  SDL_Rect dst = rect_;
  if (!full_screen_) {
    if (image_->get_height() <= rect_.h && image_->get_width() <= rect_.w) {
      dst.x = (rect_.w - image_->get_width()) / 2;
      dst.y = (rect_.h - image_->get_height()) / 2;
      dst.w = image_->get_width();
      dst.h = image_->get_height();
    } else if (image_->get_height() > rect_.h) {
      dst.w = image_->get_width() * rect_.h / image_->get_height();
      dst.h = rect_.h;
      dst.x = (rect_.w - dst.w) / 2;
    } else {
      dst.w = rect_.w;
      dst.h = image_->get_height() * rect_.w / image_->get_width();
      dst.y = (rect_.h - dst.h) / 2;
    }
  }
  return image_->render_image(nullptr, &dst);
}