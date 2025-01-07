
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
  first_line_ = 0;
  SDL_Rect line_rect = {rect_->get_rect().x, rect_->get_rect().y,
                        rect_->get_rect().w, text_height_};
  selected_rect_ = new SdlRect(renderer, line_rect);
  hightlight_rect_ = new SdlRect(renderer, line_rect);
  action_rect_ = {0, 0, 0, 0};
  selected_line_ = -1;
  last_selected_line_ = -1;
  hightlight_line_ = -1;
  entered_dir_name_ = nullptr;
  played_file_name_ = nullptr;
  audio_ = nullptr;
  video_ = nullptr;
  picture_ = nullptr;
  dir_ = nullptr;
  play_ = nullptr;
  enter_ = nullptr;
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
  selected_line_ = -1;
  hightlight_line_ = -1;
  last_selected_line_ = -1;
  vec_info_ = vec;
  text_count_ = vec_info_->size();
  if (!audio_) audio_ = new SdlImage(renderer_, "audio.png");
  if (!video_) video_ = new SdlImage(renderer_, "video.png");
  if (!picture_) picture_ = new SdlImage(renderer_, "picture.png");
  if (!dir_) dir_ = new SdlImage(renderer_, "dir.png");
  if (!unkown_) unkown_ = new SdlImage(renderer_, "unkown.png");
  if (!play_) play_ = new SdlImage(renderer_, "resume.png");
  if (!enter_) enter_ = new SdlImage(renderer_, "enter.png");
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
      if (vec_info_ && last_selected_line_ == selected_line_) {
        action_rect_.y = rect_->get_rect().y +
                         text_height_ * (selected_line_ - first_line_) - 5;
        int file_type = (*vec_info_)[selected_line_]->file_type;
        if (file_type == 0 &&
            point_in_rect(e->button.x, e->button.y, &action_rect_)) {
          entered_dir_name_ = (*vec_info_)[selected_line_]->file_name;
        } else if ((file_type == 1 || file_type == 2 || file_type == 3) &&
                   point_in_rect(e->button.x, e->button.y, &action_rect_)) {
          played_file_name_ = (*vec_info_)[selected_line_]->file_name;
        }
      }
      last_selected_line_ = selected_line_;
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

int SdlTextBox::get_action_line_name(char **name) {
  int ret = -1;
  if (entered_dir_name_ != nullptr) {
    *name = entered_dir_name_;
    ret = 0;
  } else if (played_file_name_ != nullptr) {
    *name = played_file_name_;
    ret = 1;
  } else {
    *name = nullptr;
  }
  return ret;
}

void SdlTextBox::get_line_info(int *display_line_cnt, int *text_line_cnt) {
  *display_line_cnt = rect_->get_rect().h / text_height_;
  *text_line_cnt = text_count_;
}

const char *SdlTextBox::get_next_url() {
  if (selected_line_ > text_count_ - 1) {
    return nullptr;
  }
  LOGI(TAG, "last selected line: %d", selected_line_);
  int i = selected_line_ + 1;
  while (i < text_count_) {
    if ((*vec_info_)[i]->file_type == (*vec_info_)[selected_line_]->file_type) {
      selected_line_ = i;
      break;
    } else {
      i++;
    }
  }
  LOGI(TAG, "new selected line: %d", selected_line_);
  if (i == text_count_) {
    return nullptr;
  } else {
    played_file_name_ = (*vec_info_)[selected_line_]->file_name;
    return played_file_name_;
  }

  return nullptr;
}

const char *SdlTextBox::get_prev_url() {
  if (selected_line_ <= 0) {
    return nullptr;
  }
  LOGI(TAG, "last selected line: %d", selected_line_);
  int i = selected_line_ - 1;
  while (i >= 0) {
    if ((*vec_info_)[i]->file_type == (*vec_info_)[selected_line_]->file_type) {
      selected_line_ = i;
      break;
    } else {
      i--;
    }
  }
  LOGI(TAG, "new selected line: %d", selected_line_);
  if (i < 0) {
    return nullptr;
  }

  played_file_name_ = (*vec_info_)[selected_line_]->file_name;
  return played_file_name_;
}

void SdlTextBox::update_first_line(int first_line) {
  LOGD(TAG, "update first: %d", first_line);
  first_line_ = first_line;
}

int SdlTextBox::render_text_box() {
  played_file_name_ = nullptr;
  entered_dir_name_ = nullptr;
  rect_->render_rect();
  rect_->render_edge();
  bool selected = false;
  int line_count = rect_->get_rect().h / text_height_;
  for (int i = 0; i < line_count && i + first_line_ < text_count_; ++i) {
    if (i == selected_line_ - first_line_) {
      selected_rect_->update_rect(
          rect_->get_rect().x, rect_->get_rect().y + text_height_ * i, -1, -1);
      selected_rect_->render_rect();
      selected_rect_->render_edge();
      selected = true;
    } else if (i == hightlight_line_) {
      hightlight_rect_->update_rect(
          rect_->get_rect().x, rect_->get_rect().y + text_height_ * i, -1, -1);
      hightlight_rect_->render_rect();
      hightlight_rect_->render_edge();
      selected = false;
    } else {
      selected = false;
    }

    if (vec_info_) {
      int file_type = (*vec_info_)[i + first_line_]->file_type;
      SDL_Rect image_rect = {rect_->get_rect().x + 5,
                             rect_->get_rect().y + 5 + text_height_ * i,
                             text_height_ - 10, text_height_ - 10};
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
      if (selected) {
        action_rect_ = image_rect;
        action_rect_.y -= 5;
        action_rect_.w += 10;
        action_rect_.h += 10;
        action_rect_.x =
            rect_->get_rect().x + rect_->get_rect().w - action_rect_.w - 20;
        if (file_type == 1 || file_type == 2 || file_type == 3) {
          play_->render_image(nullptr, &action_rect_);
        } else if (file_type == 0) {
          enter_->render_image(nullptr, &action_rect_);
        }
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
