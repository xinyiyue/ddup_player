#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <limits.h>
#include <unistd.h>

#include <cassert>
#include <iostream>
#include <thread>

#include "base_util/media_type.h"
#include "gui_widget/sdl_impl/sdl_audio.h"
#include "gui_widget/sdl_impl/sdl_button.h"
#include "gui_widget/sdl_impl/sdl_dirent_window_widget.h"
#include "gui_widget/sdl_impl/sdl_image_view_panel.h"
#include "gui_widget/sdl_impl/sdl_image_widget.h"
#include "gui_widget/sdl_impl/sdl_label_widget.h"
#include "gui_widget/sdl_impl/sdl_layer.h"
#include "gui_widget/sdl_impl/sdl_playback_panel.h"
#include "gui_widget/sdl_impl/sdl_user_event.h"
#include "gui_widget/sdl_impl/sdl_video.h"
#include "gui_widget/sdl_impl/sdl_window.h"
#include "log/ddup_log.h"

using namespace std;
#define TAG "PlayerApp"
struct play_pause_bar {
  SdlButton *pause_button;
  SdlButton *play_button;
  SdlButton *reload_button;
};

struct dir_window_info {
  SDL_Renderer *renderer;
  SdlWindow *win;
  SdlFont *font;
  SdlLayer *layer;
  SdlLayer *video_layer;
  SdlLabelWidget *label;
  SdlImgWidget *logo;
  SdlDirWinWidget *dir_win;
  SdlImgWidget *image;
  SdlGif *gif;
  SdlVideo *video_widget;
  SdlAudio *audio_widget;
  SdlPlaybackPanel *prog_bar;
  struct play_pause_bar *ppb;
};

struct play_pause_bar ppb;

void handle_bar_event(void *userdata, void *event) {
  action *a = (action *)event;
  struct dir_window_info *winfo = (struct dir_window_info *)userdata;
  struct play_pause_bar *ppb = winfo->ppb;

  if (a->state == PLAYBACK_PAUSE) {
    LOGI(TAG, "bar_event handler,state:%d,pause----------------------->",
         a->state);
    if (winfo->video_widget &&
        winfo->video_widget->get_state() != DDUP_STATE_STOP) {
      winfo->video_widget->set_speed(0.0);
    } else if (winfo->audio_widget &&
               winfo->audio_widget->get_state() != DDUP_STATE_STOP) {
      winfo->audio_widget->set_speed(0.0);
    }
    ppb->pause_button->set_show(true, 0);
    ppb->play_button->set_show(false, 0);
  } else if (a->state == PLAYBACK_PLAY) {
    LOGI(TAG, "bar_event handler,state:%d,play----------------------->",
         a->state);
    if (a->seek_time == 0) {
      if (winfo->video_widget &&
          winfo->video_widget->get_state() != DDUP_STATE_STOP) {
        winfo->video_widget->seek(0);
      } else if (winfo->audio_widget &&
                 winfo->audio_widget->get_state() != DDUP_STATE_STOP) {
        winfo->audio_widget->seek(0);
      }
      ppb->reload_button->set_show(false, 0);
      ppb->play_button->set_show(true, 1500);
    } else {
      ppb->pause_button->set_show(false, 0);
      ppb->play_button->set_show(true, 1500);
    }
    if (winfo->video_widget &&
        winfo->video_widget->get_state() != DDUP_STATE_STOP) {
      winfo->video_widget->set_speed(1.0);
    } else if (winfo->audio_widget &&
               winfo->audio_widget->get_state() != DDUP_STATE_STOP) {
      winfo->audio_widget->set_speed(1.0);
    }
  } else if (a->state == PLAYBACK_EOS) {
    LOGI(TAG, "bar_event handler,state:%d,eos", a->state);
    ppb->reload_button->set_show(true, 0);
  } else if (a->state == PLAYBACK_SEEK) {
    LOGI(TAG, "bar_event handler,state:%d,seek, seek_time:%lld", a->state,
         a->seek_time);
    if (winfo->video_widget &&
        winfo->video_widget->get_state() != DDUP_STATE_STOP) {
      winfo->video_widget->seek(a->seek_time);
    } else if (winfo->audio_widget &&
               winfo->audio_widget->get_state() != DDUP_STATE_STOP) {
      winfo->audio_widget->seek(a->seek_time);
    }
  }
}

int create_player(struct dir_window_info *winfo, char *url) {
  SdlWindow *win = winfo->win;
  SdlLayer *layer = winfo->layer;
  SdlLayer *video_layer = winfo->video_layer;

  int file_type = is_media_file(url);
  if (file_type == 1) {
    SdlVideo *video_widget =
        new SdlVideo("video_widge", win->renderer_mutex_, win->renderer_, 0, 0,
                     win->width_, win->height_);
    LOGI(TAG, "get url %s , create video widget", url);
    winfo->video_widget = video_widget;
    video_layer->add_widget(static_cast<Widget *>(video_widget));
    video_widget->open(url);
    video_widget->set_speed(1.0);
    winfo->prog_bar->set_show(true, 5);
  } else if (file_type == 2) {
    SdlAudio *audio_widget = new SdlAudio("audio_widge", winfo->gif);
    LOGI(TAG, "get url %s , create audio widget", url);
    winfo->audio_widget = audio_widget;
    video_layer->add_widget(static_cast<Widget *>(audio_widget));
    audio_widget->open(url);
    audio_widget->set_speed(1.0);
    winfo->prog_bar->set_show(true, 5);
  } else if (file_type == 3) {
    LOGI(TAG, "get url %s , create image widget", url);
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = win->width_;
    rect.h = win->height_;
    SdlImgWidget *picture_img_widget =
        new SdlImgWidget("image_viewer", win->renderer_, url, &rect, false);
    winfo->image = picture_img_widget;
    video_layer->add_widget(static_cast<Widget *>(picture_img_widget));
  }
  return 0;
}

int update_url(const char *url, struct dir_window_info *winfo) {
  if (url == nullptr) {
    return -1;
  }

  int type = is_media_file(url);
  if (type == 1) {  // video
    if (winfo->audio_widget != nullptr) {
      winfo->audio_widget->stop();
      winfo->audio_widget->set_show(false, 0);
    }

    if (winfo->image) winfo->image->set_show(false, 0);

    if (winfo->video_widget != nullptr) {
      winfo->prog_bar->force_hide(false);
      winfo->prog_bar->set_show(true, 5);
      winfo->video_widget->set_show(true, 0);
      winfo->video_widget->play_next(url);
    } else {
      winfo->prog_bar->force_hide(false);
      winfo->prog_bar->set_show(true, 5);
      create_player(winfo, (char *)url);
    }
  } else if (type == 2) {  // audio
    if (winfo->video_widget) {
      winfo->video_widget->set_show(false, 0);
      winfo->video_widget->stop();
    }

    if (winfo->image) winfo->image->set_show(false, 0);

    if (winfo->audio_widget) {
      winfo->prog_bar->force_hide(false);
      winfo->prog_bar->set_show(true, 5);
      winfo->audio_widget->set_show(true, 0);
      winfo->audio_widget->play_next(url);
    } else {
      winfo->prog_bar->force_hide(false);
      winfo->prog_bar->set_show(true, 5);
      create_player(winfo, (char *)url);
    }

  } else if (type == 3) {  // image
    if (winfo->audio_widget) {
      winfo->audio_widget->stop();
      winfo->audio_widget->set_show(false, 0);
      winfo->prog_bar->force_hide(true);
    }

    if (winfo->video_widget) {
      winfo->video_widget->stop();
      winfo->video_widget->set_show(false, 0);
      LOGI(TAG, "video_widget dirty:%d ----------->",
           winfo->video_widget->is_dirty());
      winfo->prog_bar->set_show(true, 5);
      winfo->prog_bar->force_hide(true);
    }

    if (winfo->image) {
      winfo->image->set_show(true, 0);
      winfo->image->update_file(url);
    } else {
      create_player(winfo, (char *)url);
    }
  }

  return 0;
}

void handle_img_next_event(void *userdata, void *event) {
  struct dir_window_info *winfo = (struct dir_window_info *)userdata;
  const char *url = winfo->dir_win->get_next_url();
  LOGI(TAG, "get new url %s ----------->", url);
  update_url(url, winfo);
}

void handle_img_prev_event(void *userdata, void *event) {
  struct dir_window_info *winfo = (struct dir_window_info *)userdata;
  const char *url = winfo->dir_win->get_prev_url();
  LOGI(TAG, "get new url %s ----------->", url);
  update_url(url, winfo);
}

void handle_play_event(void *userdata, void *event) {
  struct dir_window_info *winfo = (struct dir_window_info *)userdata;
  SdlLayer *layer = winfo->layer;
  winfo->dir_win->set_show(false, 0);
#if 1
  SdlButton *prev_button =
      new SdlButton("prev_button", "left.png", winfo->win->renderer_, 5,
                    winfo->win->height_ / 2 - 50, 200, 200);
  SdlButton *next_button = new SdlButton(
      "next_button", "right.png", winfo->win->renderer_,
      winfo->win->width_ - 50, winfo->win->height_ / 2 - 50, 200, 200);
  prev_button->set_action_callback(handle_img_prev_event, winfo);
  next_button->set_action_callback(handle_img_next_event, winfo);

  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = winfo->win->width_;
  rect.h = winfo->win->height_;
  SdlImageViewPanel *panel = new SdlImageViewPanel("image_panel", rect);
  panel->set_buttons(prev_button, next_button);
  layer->add_widget(static_cast<Widget *>(prev_button));
  layer->add_widget(static_cast<Widget *>(next_button));
  layer->add_widget(static_cast<Widget *>(panel));
#endif

#if 1
  SdlButton *pause_button = new SdlButton(
      "pause_button", "pause.png", winfo->win->renderer_,
      winfo->win->width_ / 2 - 100, winfo->win->height_ / 2 - 100, 200, 200);
  SdlButton *play_button = new SdlButton(
      "play_button", "resume.png", winfo->win->renderer_,
      winfo->win->width_ / 2 - 100, winfo->win->height_ / 2 - 100, 200, 200);
  SdlButton *reload_button = new SdlButton(
      "reload_button", "reload.png", winfo->win->renderer_,
      winfo->win->width_ / 2 - 100, winfo->win->height_ / 2 - 100, 200, 200);

  rect.x = 0;
  rect.y = winfo->win->height_ - 20;
  rect.w = winfo->win->width_;
  rect.h = 20;
  SdlPlaybackPanel *prog_bar = new SdlPlaybackPanel(
      "prog_bar", rect, "progress_bar.png", winfo->win->renderer_);
  ppb.pause_button = pause_button;
  ppb.play_button = play_button;
  ppb.reload_button = reload_button;
  winfo->ppb = &ppb;
  winfo->prog_bar = prog_bar;
  prog_bar->set_action_callback(handle_bar_event, winfo);
  layer->add_widget(static_cast<Widget *>(pause_button));
  layer->add_widget(static_cast<Widget *>(play_button));
  layer->add_widget(static_cast<Widget *>(reload_button));
  layer->add_widget(static_cast<Widget *>(prog_bar));
#endif

#if 1
  SdlLayer *video_layer = new SdlLayer("video_layer");
  video_layer->set_zorder(1);
  video_layer->set_show(true);
  winfo->win->add_layer(static_cast<Layer *>(video_layer));
  winfo->video_layer = video_layer;
#endif

  std::string *url = (std::string *)event;
  LOGI(TAG, "get new url %s ----------->", url->c_str());
  int type = is_media_file((char *)url->c_str());
  create_player(winfo, (char *)url->c_str());
}

void handle_image_event(void *userdata, void *event) {
  // struct dir_window_info *winfo = (struct dir_window_info *)userdata;
  // winfo->image->set_show(false, 0);
}

void handle_dir_event(void *userdata, void *event) {
  struct dir_window_info *winfo = (struct dir_window_info *)userdata;

  int win_width = winfo->win->width_;
  int win_height = winfo->win->height_;
  SDL_Rect rect;
  rect.w = 800;
  rect.h = 600;
  rect.x = (win_width - rect.w) / 2;
  rect.y = (win_height - rect.h) / 2;
  SdlDirWinWidget *dwin =
      new SdlDirWinWidget(winfo->renderer, &rect, winfo->font);
  winfo->dir_win = dwin;
  dwin->set_action_callback(handle_play_event, winfo);
  char cwd[1024];

  // 调用getcwd函数获取当前工作目录
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("当前路径是: %s\n", cwd);
  } else {
    perror("getcwd() 错误");
    return;
  }
  dwin->get()->parse_dir(cwd);
  LOGI(TAG, "new SdlDirWinWidget %p ----------->", dwin);
  winfo->layer->add_widget(static_cast<Widget *>(dwin));
  winfo->label->set_show(false, 0);
  winfo->logo->set_show(false, 0);
}

void resource_thread(struct dir_window_info *winfo) {
  LOGI(TAG, "resource_thread %p ----------->", winfo);
  SdlGif *gif_ = new SdlGif("gif", "music.gif", winfo->win->renderer_mutex_,
                            winfo->win->renderer_, 0, 0, winfo->win->width_,
                            winfo->win->height_);
  winfo->gif = gif_;
  LOGI(TAG, "resource_thread %p, start render gif ----------->", winfo);
  gif_->decode_gif();
  LOGI(TAG, "resource_thread %p, end render gif, exit ----------->", winfo);
}

int main(int argc, char *argv[]) {
  struct dir_window_info winfo;
  SdlWindow *win = new SdlWindow("DDup Player", 1280, 720);
  win->create();
  SdlLayer *layer = new SdlLayer("button_layer");
  layer->set_zorder(2);
  layer->set_show(true);

  memset(&winfo, 0, sizeof(winfo));
  winfo.renderer = win->renderer_;
  winfo.layer = layer;
  winfo.win = win;
  winfo.video_widget = nullptr;
  winfo.audio_widget = nullptr;
  std::thread resource_thread_id_ =
      std::thread(std::bind(resource_thread, &winfo));
  resource_thread_id_.detach();

  SDL_Rect rect;
  rect.x = win->width_ / 2 - 150;
  rect.y = win->height_ / 2 - 200;
  rect.w = 300;
  rect.h = 250;

  SdlImgWidget *img_widget =
      new SdlImgWidget("logo", win->renderer_, "ddup_player.png", &rect);
  winfo.logo = img_widget;
  img_widget->set_action_callback(handle_image_event, &winfo);

  SdlFont *font1 = new SdlFont(win->renderer_, "ddup_font.ttf", 20);
  winfo.font = font1;
  rect.x += 50;
  rect.y += 255;
  rect.w = 200;
  rect.h = 40;

  SdlLabelWidget *label_widget =
      new SdlLabelWidget(win->renderer_, font1, " Open Media file ", &rect);
  winfo.label = label_widget;
  label_widget->set_action_callback(handle_dir_event, &winfo);
  layer->add_widget(static_cast<Widget *>(label_widget));
  layer->add_widget(static_cast<Widget *>(img_widget));
  win->add_layer(static_cast<Layer *>(layer));
  win->show();

  delete win;
  return 0;
}
