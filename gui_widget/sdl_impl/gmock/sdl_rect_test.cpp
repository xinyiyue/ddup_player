#include "gui_widget/sdl_impl/sdl_rect.h"

#include <gtest/gtest.h>

class SdlRectTest : public ::testing::Test {
 protected:
  SDL_Rect rect_;
  SDL_Color color_;
  SDL_Color edge_color_;
  SdlRect* sdl_rect_;
  virtual void SetUp() {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(800, 600, 0, &window_, &renderer_);
    rect_.x = 0;
    rect_.y = 0;
    rect_.w = 200;
    rect_.h = 100;
    color_.r = 100;
    color_.g = 200;
    color_.b = 150;
    color_.a = 0;
    edge_color_ = {255, 0, 0, 0};
    sdl_rect_ = new SdlRect(renderer_, rect_, color_, 10, edge_color_);
  }

  virtual void TearDown() {
    delete sdl_rect_;
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
  }
  SDL_Window* window_;
  SDL_Renderer* renderer_;
};

TEST_F(SdlRectTest, render_rect) {
  EXPECT_EQ(sdl_rect_->render_rect(), 0);
  SDL_RenderPresent(renderer_);
  SDL_Delay(2000);
  EXPECT_EQ(sdl_rect_->render_edge(), 0);
  SDL_RenderPresent(renderer_);
  SDL_Delay(3000);
}
