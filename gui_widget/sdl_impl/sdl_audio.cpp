
#include "gui_widget/sdl_impl/sdl_audio.h"

#include <iostream>

#include "gui_widget/sdl_impl/sdl_user_event.h"
#include "log/ddup_log.h"

#define TAG "SdlAudio"

using namespace std;

SdlAudio::SdlAudio(const char *name, const char *gif, SDL_mutex *renderer_mutex,
                   SDL_Renderer *renderer, int x, int y, int w, int h)
    : EventListener("SdlAudio") {
  renderer_ = renderer;
  listener_ = nullptr;
  last_pos_ = 0;
  player_ = new DDupPlayer(static_cast<EventListener *>(this));
  gif_ = new SdlGif(name, gif, renderer_mutex, renderer_, x, y, w, h);
  LOGI(TAG, "%s", "start render gif");
  gif_->decode_gif();
  gif_->render_gif();
}

SdlAudio::~SdlAudio() {
  player_->stop();
  player_->close();
  if (player_) delete player_;
  if (gif_) delete gif_;
}

int SdlAudio::open(const char *url) {
  player_->open();
  player_->prepare(url);
  return 0;
}

int SdlAudio::set_speed(float speed) {
  gif_->set_speed(speed);
  return player_->set_speed(speed);
}

int SdlAudio::stop() {
  LOGI(TAG, "%s", "sdl video stop");
  return player_->stop();
}

int SdlAudio::seek(long long seek_time) { return player_->seek(seek_time); }

int SdlAudio::close() {
  LOGI(TAG, "%s", "sdl video close");
  player_->close();
  return 0;
}

int SdlAudio::draw() {
  LOGD(TAG, "%s", "sdl video render texutre");
  gif_->draw();
  return 0;
}

int SdlAudio::get_type() { return 0; }

int SdlAudio::event_handler(void *event) {
  dirty_ = gif_->dirty_;
  return gif_->event_handler(event);
}

void SdlAudio::notify_event(int event_type, void *ret) {
  ddup_event_t dd_event = (ddup_event_t)event_type;
  SDL_Event event;
  switch (dd_event) {
    case DDUP_EVENT_POSITION:
      real_time_pos_ = *(long long *)ret;
      if (real_time_pos_ - last_pos_ > 900) {
        event.type = SDL_USER_EVENT_POSITION_UPDATE;
        event.user.data1 = &real_time_pos_;
        event.user.data2 = NULL;
        SDL_PushEvent(&event);
        last_pos_ = real_time_pos_;
        LOGI(TAG, "notify SDL_USER_EVENT_POSITION_UPDATE:%lld", real_time_pos_);
      }
      break;
    case DDUP_EVENT_DURATION:
      duration_ = *(long long *)ret;
      event.type = SDL_USER_EVENT_DURATION_UPDATE;
      event.user.data1 = (void *)&duration_;
      event.user.data2 = NULL;
      SDL_PushEvent(&event);
      LOGD(TAG, "notify SDL_USER_EVENT_DURATION_UPDATE:%lld", duration_);
      break;
    case DDUP_EVENT_EOS:
      last_pos_ = 0;
      event.type = SDL_USER_EVENT_EOS_UPDATE;
      event.user.data1 = NULL;
      event.user.data2 = NULL;
      SDL_PushEvent(&event);
      LOGD(TAG, "%s", "notify SDL_USER_EVENT_EOS_UPDATE");
      break;

    default:
      if (listener_) listener_->notify_event(event_type, ret);
      break;
  };
}

void SdlAudio::notify_error(int error_type) {
  if (listener_) listener_->notify_error(error_type);
}

int SdlAudio::set_event_resp_area(int x, int y, int w, int h) { return 0; }

void *SdlAudio::get_window() { return nullptr; }

void *SdlAudio::get_renderer() { return renderer_; }
