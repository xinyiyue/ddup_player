
#include "./gui_widget/sdl_impl/sdl_text_box.h"

#include "./log/ddup_log.h"

#define TAG "SdlTextBox"

SdlTextBox::SdlTextBox(SDL_Renderer *renderer, SDL_Rect &rect, SdlFont *font) {
  renderer_ = renderer;
  rect_ = new SdlRect(renderer, rect);
  font_ = font;
  text_height_ = 0;
  text_array_ = nullptr;
  vec_info_ = nullptr;
  first_line_ = -1;
  SDL_Rect line_rect = {rect_->get_rect().x, rect_->get_rect().y,
                        rect_->get_rect().w, text_height_};
  selected_rect_ = new SdlRect(renderer, line_rect);
  hightlight_rect_ = new SdlRect(renderer, line_rect);
  selected_line_ = -1;
  hightlight_line_ = -1;

  audio_ = nullptr;
  video_ = nullptr;
  picture_ = nullptr;
  dir_ = nullptr;
}

SdlTextBox::~SdlTextBox() {
  if (!audio_) delete audio_;
  if (!video_) delete video_;
  if (!picture_) delete picture_;
  if (!dir_) delete dir_;
}

void SdlTextBox::set_info(int text_hight, SDL_Color *text,
                          SDL_Color *background, SDL_Color *hightlight,
                          SDL_Color *selected) {
  text_height_ = text_hight;
  text_color_ = *text;
  rect_->set_attri(background, 1, background);
  selected_rect_->set_attri(selected, 1, selected);
  selected_rect_->update_rect(-1, -1, -1, text_height_);
  hightlight_rect_->set_attri(hightlight, 1, hightlight);
  hightlight_rect_->update_rect(-1, -1, -1, text_height_);
  return;
}

void SdlTextBox::set_dirent_info(std::vector<DirentInfo *> *vec) {
  vec_info_ = vec;
  text_count_ = vec_info_->size();
  if (!audio_) audio_ = new SdlImage(renderer_, "audio.png");
  if (!video_) video_ = new SdlImage(renderer_, "video.png");
  if (!picture_) picture_ = new SdlImage(renderer_, "picture.png");
  if (!dir_) dir_ = new SdlImage(renderer_, "dir.png");
  if (!unkown_) unkown_ = new SdlImage(renderer_, "unkown.png");
  return;
}

void SdlTextBox::set_text(const char **text_array, int count) {
  text_array_ = text_array;
  text_count_ = count;
  return;
}

int SdlTextBox::event_handler(void *event) {
  SDL_Event *e = (SDL_Event *)event;
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    if (point_in_rect(e->button.x, e->button.y, &rect_->get_rect())) {
      selected_line_ = (e->button.y - rect_->get_rect().y) / text_height_;
      if (first_line_) selected_line_ += first_line_;
    }
  } else if (e->type == SDL_MOUSEMOTION) {
    if (point_in_rect(e->motion.x, e->motion.y, &rect_->get_rect())) {
      hightlight_line_ = (e->motion.y - rect_->get_rect().y) / text_height_;
    } else {
      hightlight_line_ = -1;
    }
  }
  return 0;
}

void SdlTextBox::get_line_info(int *display_line_cnt, int *text_line_cnt) {
  *display_line_cnt = rect_->get_rect().h / text_height_;
  *text_line_cnt = text_count_;
}

void SdlTextBox::update_first_line(int first_line) {
  LOGI(TAG, "update first: %d", first_line);
  first_line_ = first_line;
}

int SdlTextBox::render_text_box() {
  rect_->render_rect();
  rect_->render_edge();
  int line_count = rect_->get_rect().h / text_height_;
  for (int i = 0; i < line_count && i + first_line_ < text_count_; ++i) {
    if (i == selected_line_ - first_line_) {
      selected_rect_->update_rect(
          rect_->get_rect().x, rect_->get_rect().y + text_height_ * i, -1, -1);
      selected_rect_->render_rect();
      selected_rect_->render_edge();
    } else if (i == hightlight_line_) {
      hightlight_rect_->update_rect(
          rect_->get_rect().x, rect_->get_rect().y + text_height_ * i, -1, -1);
      hightlight_rect_->render_rect();
      hightlight_rect_->render_edge();
    }

    if (vec_info_) {
      int file_type = (*vec_info_)[i + first_line_]->file_type;
      SDL_Rect image_rect = {rect_->get_rect().x + 5,
                             rect_->get_rect().y + 5 + text_height_ * i,
                             text_height_ - 10, text_height_ - 10};
      // LOGI(TAG, "file name:%s, type: %d", (*vec_info_)[i]->file_name,
      // file_type);
      switch (file_type) {
        case 0:
          dir_->render_image(nullptr, &image_rect);
          break;
        case 1:
          video_->render_image(nullptr, &image_rect);
          break;
        case 2:
          audio_->render_image(nullptr, &image_rect);
          break;
        case 3:
          picture_->render_image(nullptr, &image_rect);
          break;
        default:
          unkown_->render_image(nullptr, &image_rect);
          break;
      }
      font_->render_text((*vec_info_)[i + first_line_]->file_name,
                         rect_->get_rect().x + text_height_ + 5,
                         rect_->get_rect().y + text_height_ * i, text_color_);
    } else {
      font_->render_text(text_array_[i + first_line_], rect_->get_rect().x,
                         rect_->get_rect().y + text_height_ * i, text_color_);
    }
  }
  return 0;
}