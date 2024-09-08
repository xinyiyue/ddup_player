#include "gui_widget/sdl_impl/sdl_vscroll_bar.h"
#include "third_party/GoogleTest/include/gmock/gmock.h"
#include "third_party/GoogleTest/include/gtest/gtest.h"

class SdlVScrollBarTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    SDL_CreateWindowAndRenderer(800, 600, 0, &window_, &renderer_);
    rect_.x = 10;
    rect_.y = 10;
    rect_.w = 50;
    rect_.h = 500;
    vscroll_ = new SdlVScrollBar(renderer_, &rect_);
  }

  virtual void TearDown() {
    SDL_DestroyWindow(window_);
    SDL_DestroyRenderer(renderer_);
    IMG_Quit();
    SDL_Quit();
  }

  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  SdlVScrollBar* vscroll_;
  SDL_Rect rect_;
};
#if 0
TEST_F(SdlVScrollBarTest, event_handler_up_button) {
  vscroll_->render_vscroll();
  SDL_RenderPresent(renderer_);
  SDL_Delay(1000);
  int count = 10;
  while(count > 0) {
    SDL_Event event;
    event.type = SDL_MOUSEBUTTONDOWN;
    event.button.x = 15;
    event.button.y = 475;
    EXPECT_EQ(vscroll_->event_handler(&event), 0);
    //EXPECT_EQ(vscroll_->slider_rect_.y, -vscroll_->step_);
    vscroll_->render_vscroll();
    SDL_RenderPresent(renderer_);
    count--;
    SDL_Delay(2000);
  }
  count = 10;
  while(count > 0) {
    SDL_Event event;
    event.type = SDL_MOUSEBUTTONDOWN;
    event.button.x = 15;
    event.button.y = 15;
    EXPECT_EQ(vscroll_->event_handler(&event), 0);
    //EXPECT_EQ(vscroll_->slider_rect_.y, -vscroll_->step_);
    vscroll_->render_vscroll();
    SDL_RenderPresent(renderer_);
    count--;
    SDL_Delay(2000);
  }
}
#endif
TEST_F(SdlVScrollBarTest, event_handler_slider) {
  bool quit = false;
  SDL_Event e;
  while (!quit) {
    // 处理事件
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      } else {
        vscroll_->event_handler(&e);
      }
    }
    vscroll_->render_vscroll();
    SDL_RenderPresent(renderer_);
  }
}