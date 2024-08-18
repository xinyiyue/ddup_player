#include "gui_widget/sdl_impl/sdl_progress_bar.h"

#include <gtest/gtest.h>

class SdlProgressBarTest : public ::testing::Test {
 protected:
  SdlProgressBar* bar1_;
  SdlProgressBar* bar2_;
  SdlProgressBar* bar3_;
  virtual void SetUp() {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(800, 600, 0, &window_, &renderer_);

    SDL_Rect rect1 = {0, 0, 500, 100};
    SDL_Color fill_color = {255, 0, 0, 0};
    int edge = 4;
    SDL_Color edge_color = {0, 255, 0, 0};
    bar1_ = new SdlProgressBar(renderer_, "prog_bar1", "progress_bar.png");
    bar1_->set_bar_rect(rect1, fill_color, edge, edge_color);
    bar1_->set_run(true);

    SDL_Rect rect2 = {0, 120, 500, 50};
    bar2_ = new SdlProgressBar(renderer_, "prog_bar2", "progress_bar.png");
    bar2_->set_bar_rect(rect2, fill_color, edge, edge_color);
    bar2_->set_run(true);

    SDL_Rect rect3 = {0, 190, 500, 20};
    bar3_ = new SdlProgressBar(renderer_, "prog_bar3", "progress_bar.png");
    bar3_->set_bar_rect(rect3, fill_color, edge, edge_color);
    bar3_->set_run(true);
  }

  virtual void TearDown() {
    delete bar1_;
    delete bar2_;
    delete bar3_;
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
  }
  SDL_Window* window_;
  SDL_Renderer* renderer_;
};

TEST_F(SdlProgressBarTest, render_bar) {
  while (bar1_->get_run()) {
    int ret = bar1_->render_bar();
    EXPECT_EQ(ret, 0);
    ret = bar2_->render_bar();
    EXPECT_EQ(ret, 0);
    ret = bar3_->render_bar();
    EXPECT_EQ(ret, 0);
    SDL_RenderPresent(renderer_);
    SDL_Delay(30);
  }
  SDL_Delay(3000);
}
