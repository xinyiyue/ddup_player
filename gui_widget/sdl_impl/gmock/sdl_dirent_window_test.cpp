#include "gui_widget/sdl_impl/sdl_dirent_window.h"

#include <chrono>
#include <thread>

#include "third_party/GoogleTest/include/gmock/gmock.h"
#include "third_party/GoogleTest/include/gtest/gtest.h"

class SdlDirentWindowTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    SDL_CreateWindowAndRenderer(1280, 720, 0, &window_, &renderer_);
    rect_.x = 10;
    rect_.y = 10;
    rect_.w = 800;
    rect_.h = 680;
    font_ = new SdlFont(renderer_, "ddup_font.ttf", 30);

    dwin_ = new SdlDirentWindow(renderer_, rect_, font_);
    dwin_->parse_dir("/home/mi/data/ddup_player/res");
  }

  virtual void TearDown() {
    delete font_;
    delete dwin_;
    SDL_DestroyWindow(window_);
    SDL_DestroyRenderer(renderer_);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
  }

  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  SdlFont* font_;
  SDL_Rect rect_;
  SdlDirentWindow* dwin_;
};

TEST_F(SdlDirentWindowTest, event_handler_dbox) {
  bool quit = false;
  SDL_Event e;
  while (!quit) {
    SDL_RenderClear(renderer_);
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      } else {
        dwin_->event_handler(&e);
      }
    }
    dwin_->render_dirent_window();
    SDL_RenderPresent(renderer_);
  }
}