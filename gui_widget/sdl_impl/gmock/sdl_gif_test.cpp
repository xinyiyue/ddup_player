#include "gui_widget/sdl_impl/sdl_gif.h"

#include <gtest/gtest.h>

class SdlGifTest : public ::testing::Test {
 protected:
  SdlGif* gif_;
  SDL_mutex* renderer_mutex_;
  virtual void SetUp() {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(800, 600, 0, &window_, &renderer_);
    renderer_mutex_ = SDL_CreateMutex();
    gif_ = new SdlGif("gif_widget", "music.gif", renderer_mutex_, renderer_, 0,
                      0, 800, 400);
  }

  virtual void TearDown() {
    delete gif_;
    SDL_DestroyMutex(renderer_mutex_);
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
  }
  SDL_Window* window_;
  SDL_Renderer* renderer_;
};

TEST_F(SdlGifTest, gif_test) {
  bool quit = false;
  SDL_Event e;
  gif_->decode_gif();
  gif_->render_gif();
  while (!quit) {
    // 处理事件
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      } else {
        gif_->event_handler(&e);
      }
    }
    AutoLock lock(renderer_mutex_);
    SDL_RenderClear(renderer_);
    gif_->draw();
    SDL_RenderPresent(renderer_);
    // SDL_Delay(20);
  }
}
