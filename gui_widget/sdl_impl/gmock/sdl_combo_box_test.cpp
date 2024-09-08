#include "gui_widget/sdl_impl/sdl_combo_box.h"

#include "third_party/GoogleTest/include/gmock/gmock.h"
#include "third_party/GoogleTest/include/gtest/gtest.h"

class SdlComboBoxTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    SDL_CreateWindowAndRenderer(800, 600, 0, &window_, &renderer_);
    static const char *text[] = {"sdl",      "text", "box", "test", "program",
                                 "hahahaha", "111",  "222", "333",  "444",
                                 "555",      "666",  "777", "888",  "999",
                                 "1010",     "1111", "1212"};
    rect_.x = 10;
    rect_.y = 10;
    rect_.w = 500;
    rect_.h = 400;
    font_ = new SdlFont(renderer_, "ddup_font.ttf", 30);
#if 1
    text_box_ = new SdlTextBox(renderer_, rect_, font_);
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color gray = {150, 150, 150, 0};
    SDL_Color white = {200, 200, 200, 255};
    SDL_Color blue = {173, 216, 230, 50};
    text_box_->set_info(30, &black, &gray, &white, &blue);
    text_box_->set_text(text, 18);
    slider_rect_.x = 510;
    slider_rect_.y = 10;
    slider_rect_.w = 30;
    slider_rect_.h = 400;
    vscroll_ = new SdlVScrollBar(renderer_, &slider_rect_);
    cbox_ = new SdlComboBox(renderer_, font_, text_box_, vscroll_);
#else
    cbox_ = new SdlComboBox(renderer_, rect_, font_);
    cbox_->set_text(text, 18);
#endif
  }

  virtual void TearDown() {
    delete font_;
    delete text_box_;
    delete vscroll_;
    delete cbox_;
    SDL_DestroyWindow(window_);
    SDL_DestroyRenderer(renderer_);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
  }

  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;
  SdlTextBox *text_box_;
  SdlFont *font_;
  SDL_Rect rect_;
  SDL_Rect slider_rect_;
  SdlVScrollBar *vscroll_;
  SdlComboBox *cbox_;
};

TEST_F(SdlComboBoxTest, event_handler_cbox) {
  bool quit = false;
  SDL_Event e;
  while (!quit) {
    // 处理事件
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      } else {
        cbox_->event_handler(&e);
      }
    }
    cbox_->render_combo_box();
    SDL_RenderPresent(renderer_);
  }
}