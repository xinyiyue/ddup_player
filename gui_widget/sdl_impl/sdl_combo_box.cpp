
#include "./gui_widget/sdl_impl/sdl_combo_box.h"

#include "./log/ddup_log.h"
#include "log/ddup_log.h"

#define TAG "SdlComboBox"

SdlComboBox::SdlComboBox(SDL_Renderer *renderer, SDL_Rect &rect,
                         SdlFont *font) {
  renderer_ = renderer;
  font_ = font;
  SDL_Color black = {50, 50, 50, 255};
  SDL_Color gray = {235, 235, 235, 255};
  SDL_Color white = {255, 255, 255, 255};
  int slide_width = rect.w / 20 > 30 ? 30 : 20;
  SDL_Rect text_rect = {rect.x, rect.y, rect.w - slide_width, rect.h};
  LOGI(TAG, "text width:%d, sild x:%d", text_rect.w, rect.x + text_rect.w);
  SDL_Rect slide_rect = {rect.x + text_rect.w, rect.y, slide_width, rect.h};
  text_box_ = new SdlTextBox(renderer_, text_rect, font_);
  text_box_->set_info(font_->get_font_height(), &black, &white, &gray,
                      &baby_blue);
  vscroll_bar_ = new SdlVScrollBar(renderer_, &slide_rect);
}

SdlComboBox::SdlComboBox(SDL_Renderer *renderer, SdlFont *font,
                         SdlTextBox *text_box, SdlVScrollBar *vscroll_bar) {
  renderer_ = renderer;
  font_ = font;
  text_box_ = text_box;
  vscroll_bar_ = vscroll_bar;
  int display_cnt;
  int total_cnt;
  text_box_->get_line_info(&display_cnt, &total_cnt);
  if (total_cnt > display_cnt) {
    vscroll_bar_->update_slider_hight((1.0 * display_cnt) / total_cnt);
    need_srcollbar_ = true;
  } else {
    need_srcollbar_ = false;
  }
  LOGI(TAG, "need_srcollbar_: %d", need_srcollbar_);
}

SdlComboBox::~SdlComboBox() {}

void SdlComboBox::set_dirent_info(std::vector<DirentInfo *> *vec) {
  text_box_->set_dirent_info(vec);
  int display_cnt;
  int total_cnt;
  text_box_->get_line_info(&display_cnt, &total_cnt);
  if (total_cnt > display_cnt) {
    vscroll_bar_->update_slider_hight((1.0 * display_cnt) / total_cnt);
    need_srcollbar_ = true;
  } else {
    need_srcollbar_ = false;
  }
}

void SdlComboBox::set_text(const char **text_array, int count) {
  text_box_->set_text(text_array, count);
  int display_cnt;
  int total_cnt;
  text_box_->get_line_info(&display_cnt, &total_cnt);
  if (total_cnt > display_cnt) {
    vscroll_bar_->update_slider_hight((1.0 * display_cnt) / total_cnt);
    need_srcollbar_ = true;
  } else {
    need_srcollbar_ = false;
  }
}

int SdlComboBox::event_handler(void *event) {
  text_box_->event_handler(event);
  if (need_srcollbar_) {
    vscroll_bar_->event_handler(event);
    int display_cnt;
    int total_cnt;
    text_box_->get_line_info(&display_cnt, &total_cnt);
    double up_percent = vscroll_bar_->get_slider_up_space();
    int first_line = display_cnt * up_percent;
    text_box_->update_first_line(first_line);
  }
  return 0;
}

int SdlComboBox::render_combo_box() {
  if (need_srcollbar_) {
    vscroll_bar_->render_vscroll();
  }
  text_box_->render_text_box();
  return 0;
}