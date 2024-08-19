#include "gui_widget/sdl_impl/sdl_label.h"

#include "third_party/GoogleTest/include/gmock/gmock.h"
#include "third_party/GoogleTest/include/gtest/gtest.h"

class SdlLabelTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    SDL_CreateWindowAndRenderer(800, 600, 0, &window_, &renderer_);
  }

  virtual void TearDown() {
    SDL_DestroyWindow(window_);
    SDL_DestroyRenderer(renderer_);
    TTF_Quit();
    SDL_Quit();
  }

  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
};

TEST_F(SdlLabelTest, SdlLabelTest) {
  SdlFont* font = new SdlFont(renderer_, "ddup_font.ttf");
  SdlFont* font2 = new SdlFont(renderer_, "ddup_font.ttf", 30);
  int edge_width = 1;
  SDL_Color edge_color = {255, 0, 0, 0};
  SDL_Color font_color = {255, 255, 255, 255};
  SDL_Color bg_color = {0, 0, 255, 0};

  SdlLabel* label = new SdlLabel(renderer_, font, "DAY DAY UP1", 10, 10, true);
  label->set_attri(&font_color, &bg_color, &edge_color, edge_width);

  SdlLabel* label2 =
      new SdlLabel(renderer_, font2, "DAY DAY UP2", 10, 50, false);

  SdlLabel* label3 =
      new SdlLabel(renderer_, font2, "DAY DAY UP3", 10, 100, true);

  SdlLabel* label4 =
      new SdlLabel(renderer_, font2, "DAY DAY UP4", 10, 150, true);
  label4->set_attri(&font_color, &bg_color, &edge_color, edge_width);

  label->render_label();
  label2->render_label();
  label3->render_label();
  label4->render_label();
  SDL_RenderPresent(renderer_);
  SDL_Delay(10000);
  delete label;
  delete label2;
  delete label3;
  delete label4;
  delete font;
}