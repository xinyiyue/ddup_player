
#include "gui_widget/sdl_impl/sdl_video.h"

#include <iostream>

#include "gui_widget/sdl_impl/sdl_user_event.h"
#include "log/ddup_log.h"

#define TAG "SdlVideo"

using namespace std;

SdlVideo::SdlVideo(const char *name, kiss_array *arr, kiss_window *win,
                   SDL_mutex *renderer_mutex, SDL_Renderer *renderer, int x,
                   int y, int w, int h)
    : Video(name),
      EventListener("SdlVideo"),
      SdlTextureBuilder("SdlVideoTextureBuilder", renderer_mutex, renderer, x,
                        y, w, h) {
  renderer_ = renderer;
  window_ = win;
  exit_ = false;
  listener_ = nullptr;
  dirty_ = true;
  player_ = new DDupPlayer(static_cast<EventListener *>(this));
}

SdlVideo::~SdlVideo() {
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

int SdlVideo::draw() {
  LOGD(TAG, "%s", "sdl video render texutre");
  render_texture();
  return 0;
}

int SdlVideo::get_type() { return 0; }

int SdlVideo::event_handler(void *event) {
  SDL_Event *e = (SDL_Event *)event;
  if (e->type == SDL_USER_EVENT_VIDEO_UPDATE) {
    LOGD(TAG, "%s",
         "catch event video update event, will update video picture");
    return 1;
  } else if (e->type == SDL_QUIT) {
    LOGI(TAG, "%s", "catch SDL QUIT event, will stop and close");
    stop();
    close();
  }
  return 0;
}

void SdlVideo::notify_event(int event_type, void *ret) {
  ddup_event_t dd_event = (ddup_event_t)event_type;
  SDL_Event event;
  switch (dd_event) {
    case DDUP_EVENT_POSITION:
      position_ = *(long long *)ret;
      event.type = SDL_USER_EVENT_POSITION_UPDATE;
      event.user.data1 = &position_;
      event.user.data2 = NULL;
      SDL_PushEvent(&event);
      LOGD(TAG, "notify SDL_USER_EVENT_POSITION_UPDATE:%lld",
           position_);
      break;
    case DDUP_EVENT_DURATION:
      duration_ = *(long long *)ret;
      event.type = SDL_USER_EVENT_DURATION_UPDATE;
      event.user.data1 = (void *)&duration_;
      event.user.data2 = NULL;
      SDL_PushEvent(&event);
      LOGD(TAG, "notify SDL_USER_EVENT_DURATION_UPDATE:%lld",
           duration_);
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

void *SdlVideo::get_window() { return window_; }

void *SdlVideo::get_renderer() { return renderer_; }
