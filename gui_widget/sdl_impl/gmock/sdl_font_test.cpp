#include "gui_widget/sdl_impl/sdl_font.h"

#include "third_party/GoogleTest/include/gmock/gmock.h"
#include "third_party/GoogleTest/include/gtest/gtest.h"

class SdlFontTest : public ::testing::Test {
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

TEST_F(SdlFontTest, SdlFontRenderSrcRect) {
  SdlFont* font = new SdlFont(renderer_, "ddup_font.ttf");
  SDL_Color r = {255, 0, 0, 0};
  font->get_text_rect("day day up player");
  font->render_text("day day up player", 0, 0, r);
  SDL_RenderPresent(renderer_);
  SDL_Delay(1000);

  SDL_Color g = {0, 255, 0, 0};
  SDL_Rect dst_rect = {0, 30, 600, 100};
  font->render_text("day day up player", dst_rect, g);
  SDL_RenderPresent(renderer_);
  SDL_Delay(1000);
  delete font;

  SdlFont font2(renderer_, "ddup_font.ttf", 30);
  SDL_Color b = {0, 0, 255, 0};
  font2.render_text("day day up player", 250, 0, b);
  SDL_RenderPresent(renderer_);

  SDL_Delay(3000);
}