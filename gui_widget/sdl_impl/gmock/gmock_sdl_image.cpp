#include "gui_widget/sdl_impl/sdl_image.h"
#include "third_party/GoogleTest/include/gmock/gmock.h"
#include "third_party/GoogleTest/include/gtest/gtest.h"

class SdlImageTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    SDL_CreateWindowAndRenderer(800, 600, 0, &window_, &renderer_);
  }

  virtual void TearDown() {
    SDL_DestroyWindow(window_);
    SDL_DestroyRenderer(renderer_);
    IMG_Quit();
    SDL_Quit();
  }

  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
};

TEST_F(SdlImageTest, SdlImageRenderSrcRect) {
  SdlImage image(renderer_, "pause.png");

  SDL_Rect src_rect = {0, 0, 100, 100};
  image.render_image(&src_rect, 0, 0);

  SDL_RenderPresent(renderer_);
  SDL_Delay(1000);
  image.render_image(nullptr, image.get_width() + 10, 0);
  SDL_RenderPresent(renderer_);
  SDL_Delay(1000);

  SDL_Rect dst_rect = {image.get_width() * 2 + 10, image.get_height() + 10,
                       image.get_width() * 2, image.get_height() * 2};
  image.render_image(nullptr, &dst_rect);
  SDL_RenderPresent(renderer_);
  SDL_Delay(3000);
}