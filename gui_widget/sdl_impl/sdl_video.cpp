
#include "gui_widget/sdl_impl/sdl_video.h"

#include <iostream>

#include "gui_widget/sdl_impl/sdl_user_event.h"
#include "log/ddup_log.h"

#define TAG "SdlVideo"

using namespace std;

SdlVideo::SdlVideo(const char *name, SDL_mutex *renderer_mutex,
                   SDL_Renderer *renderer, int x, int y, int w, int h)
    : Video(name),
      EventListener("SdlVideo"),
      SdlTextureBuilder("SdlVideoTextureBuilder", renderer_mutex, renderer, x,
                        y, w, h) {
  renderer_ = renderer;
  exit_ = false;
  listener_ = nullptr;
  dirty_ = true;
  last_pos_ = 0;
  player_ = new DDupPlayer(static_cast<EventListener *>(this));
}

SdlVideo::~SdlVideo() {
  player_->stop();
  player_->close();
  if (player_) delete player_;
}

int SdlVideo::open(const char *url) {
  player_->open();
  player_->prepare(url);
  return 0;
}

int SdlVideo::set_speed(float speed) { return player_->set_speed(speed); }

int SdlVideo::stop() {
  LOGI(TAG, "%s", "sdl video stop");
  return player_->stop();
}

int SdlVideo::seek(long long seek_time) { return player_->seek(seek_time); }

int SdlVideo::close() {
  LOGI(TAG, "%s", "sdl video close");
  player_->close();
  exit_ = true;
  return 0;
}

int SdlVideo::play_next(const char *url) {
  LOGI(TAG, "play next url:%s", url);
  player_->stop();
  player_->prepare(url);
  player_->set_speed(1.0);
  return 0;
}

int SdlVideo::draw() {
  LOGI(TAG, "%s", "sdl video render texutre");
  render_texture();
  return 0;
}

int SdlVideo::get_type() { return 0; }

int SdlVideo::event_handler(void *event) {
  SDL_Event *e = (SDL_Event *)event;
  if (e->type == SDL_USER_EVENT_VIDEO_UPDATE &&
      player_->get_state() != DDUP_STATE_STOP) {
    dirty_ = true;
    LOGD(TAG,
         "catch event video update event, will update video picture, dirty_:%d",
         dirty_);
    return 1;
  }
  return 0;
}

void SdlVideo::notify_event(int event_type, void *ret) {
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

void SdlVideo::notify_error(int error_type) {
  if (listener_) listener_->notify_error(error_type);
}

int SdlVideo::set_event_resp_area(int x, int y, int w, int h) { return 0; }

void *SdlVideo::get_window() { return nullptr; }

void *SdlVideo::get_renderer() { return renderer_; }
