#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

#include <iostream>

#include "gui_widget/sdl_impl/sdl_button.h"
#include "gui_widget/sdl_impl/sdl_dirent_window_widget.h"
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
  SdlVideo *video;
};

struct dir_window_info {
  SDL_Renderer *renderer;
  SdlWindow *win;
  SdlFont *font;
  SdlLayer *layer;
  SdlLabelWidget *label;
  SdlImgWidget *image;
  SdlDirWinWidget *dir_win;
};

struct play_pause_bar ppb;

void handle_bar_event(void *userdata, void *event) {
  action *a = (action *)event;
  struct play_pause_bar *ppb = (struct play_pause_bar *)userdata;
  if (a->state == PLAYBACK_PAUSE) {
    LOGI(TAG, "bar_event handler,state:%d,pause----------------------->",
         a->state);
    ppb->video->set_speed(0.0);
    ppb->pause_button->set_show(true, 0);
    ppb->play_button->set_show(false, 0);
  } else if (a->state == PLAYBACK_PLAY) {
    LOGI(TAG, "bar_event handler,state:%d,play----------------------->",
         a->state);
    if (a->seek_time == 0) {
      ppb->video->seek(0);
      ppb->reload_button->set_show(false, 0);
      ppb->play_button->set_show(true, 1500);
    } else {
      ppb->pause_button->set_show(false, 0);
      ppb->play_button->set_show(true, 1500);
    }
    ppb->video->set_speed(1.0);
  } else if (a->state == PLAYBACK_EOS) {
    LOGI(TAG, "bar_event handler,state:%d,eos", a->state);
    ppb->reload_button->set_show(true, 0);
  } else if (a->state == PLAYBACK_SEEK) {
    LOGI(TAG, "bar_event handler,state:%d,seek, seek_time:%lld", a->state,
         a->seek_time);
    ppb->video->seek(a->seek_time);
  }
}

int create_player(struct dir_window_info *winfo, char *url) {
  SdlWindow *win = winfo->win;
  SdlLayer *layer = winfo->layer;
  SdlLayer *video_layer = new SdlLayer("video_layer");
  video_layer->set_zorder(1);
  video_layer->set_show(true);
  SdlVideo *video_widget =
      new SdlVideo("video_widge", win->renderer_mutex_, win->renderer_, 0, 0,
                   win->width_, win->height_);
  video_layer->add_widget(static_cast<Widget *>(video_widget));
  SdlButton *pause_button =
      new SdlButton("pause_button", "pause.png", win->renderer_,
                    win->width_ / 2 - 100, win->height_ / 2 - 100, 200, 200);
  SdlButton *play_button =
      new SdlButton("play_button", "resume.png", win->renderer_,
                    win->width_ / 2 - 100, win->height_ / 2 - 100, 200, 200);
  SdlButton *reload_button =
      new SdlButton("reload_button", "reload.png", win->renderer_,
                    win->width_ / 2 - 100, win->height_ / 2 - 100, 200, 200);
  SDL_Rect rect;
  rect.x = 0;
  rect.y = win->height_ - 20;
  rect.w = win->width_;
  rect.h = 20;
  SdlPlaybackPanel *prog_bar = new SdlPlaybackPanel(
      "prog_bar", rect, "progress_bar.png", win->renderer_);
  ppb.pause_button = pause_button;
  ppb.play_button = play_button;
  ppb.reload_button = reload_button;
  ppb.video = video_widget;
  prog_bar->set_action_callback(handle_bar_event, &ppb);
  layer->add_widget(static_cast<Widget *>(pause_button));
  layer->add_widget(static_cast<Widget *>(play_button));
  layer->add_widget(static_cast<Widget *>(reload_button));
  layer->add_widget(static_cast<Widget *>(prog_bar));
  win->add_layer(static_cast<Layer *>(video_layer));
  video_widget->open(url);
  // video_widget->open("../../res/tianhou.mp4");
  video_widget->set_speed(1.0);
  return 0;
}

void handle_play_event(void *userdata, void *event) {
  struct dir_window_info *winfo = (struct dir_window_info *)userdata;
  winfo->dir_win->set_show(false, 0);
  std::string *url = (std::string *)event;
  LOGI(TAG, "get new url %s ----------->", url->c_str());
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
  dwin->get()->parse_dir("/data/ddup_player/ddup_player");
  LOGI(TAG, "new SdlDirWinWidget %p ----------->", dwin);
  winfo->layer->add_widget(static_cast<Widget *>(dwin));
  winfo->label->set_show(false, 0);
  winfo->image->set_show(false, 0);
}

int main(int argc, char *argv[]) {
  SdlWindow *win = new SdlWindow("DDup Player", 1280, 720);
  win->create();
  SdlLayer *layer = new SdlLayer("button_layer");
  layer->set_zorder(2);
  layer->set_show(true);

  SDL_Rect rect;
  rect.x = win->width_ / 2 - 150;
  rect.y = win->height_ / 2 - 200;
  rect.w = 300;
  rect.h = 250;

  struct dir_window_info winfo;

  winfo.renderer = win->renderer_;
  winfo.layer = layer;
  winfo.win = win;

  SdlImgWidget *img_widget =
      new SdlImgWidget(win->renderer_, "ddup_player.png", &rect);
  winfo.image = img_widget;
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
