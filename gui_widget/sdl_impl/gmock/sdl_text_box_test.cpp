#include "gui_widget/sdl_impl/sdl_text_box.h"

#include "third_party/GoogleTest/include/gmock/gmock.h"
#include "third_party/GoogleTest/include/gtest/gtest.h"

class SdlTextBoxTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    SDL_CreateWindowAndRenderer(800, 600, 0, &window_, &renderer_);
    rect_.x = 10;
    rect_.y = 10;
    rect_.w = 500;
    rect_.h = 500;
    font_ = new SdlFont(renderer_, "ddup_font.ttf", 30);
    text_box_ = new SdlTextBox(renderer_, rect_, font_);
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color gray = {150, 150, 150, 0};
    SDL_Color white = {200, 200, 200, 255};
    SDL_Color blue = {173, 216, 230, 50};
    text_box_->set_info(30, &black, &gray, &white, &blue);
    static const char *text[] = {"sdl",     "text",   "box",     "test",
                                 "program", "111111", "22222222"};
    text_box_->set_text(text, 7);
  }

  virtual void TearDown() {
    SDL_DestroyWindow(window_);
    SDL_DestroyRenderer(renderer_);
    TTF_Quit();
    SDL_Quit();
  }

  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;
  SdlTextBox *text_box_;
  SdlFont *font_;
  SDL_Rect rect_;
};

TEST_F(SdlTextBoxTest, event_handler_cbox) {
  bool quit = false;
  SDL_Event e;
  while (!quit) {
    // 处理事件
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      } else {
        text_box_->event_handler(&e);
      }
    }
    text_box_->render_text_box();
    SDL_RenderPresent(renderer_);
  }
}