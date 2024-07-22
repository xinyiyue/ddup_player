#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

#include <iostream>

#include "gui_widget/sdl_impl/sdl_button.h"
#include "gui_widget/sdl_impl/sdl_layer.h"
#include "gui_widget/sdl_impl/sdl_progress_bar.h"
#include "gui_widget/sdl_impl/sdl_video.h"
#include "gui_widget/sdl_impl/sdl_window.h"

using namespace std;

struct play_pause_bar {
  SdlButton *pause_button;
  SdlButton *play_button;
  SdlVideo *video;
};

void handle_bar_event(void *userdata, void *event) {
  action *a = (action *)event;
  struct play_pause_bar *ppb = (struct play_pause_bar *)userdata;
  cout << "bar_event handler,state::" << a->state
       << ", seek_time:" << a->seek_time << endl;
  if (a->state == PLAYBACK_PAUSE) {
    ppb->video->set_speed(0.0);
    ppb->pause_button->set_show(true, 0);
    ppb->play_button->set_show(false, 0);
  } else if (a->state == PLAYBACK_PLAY) {
    ppb->video->set_speed(1.0);
    ppb->pause_button->set_show(false, 0);
    ppb->play_button->set_show(true, 1500);
  } else if (a->state == PLAYBACK_SEEK) {
    ppb->video->seek(a->seek_time);
  }
}

int main(int argc, char *argv[]) {
  SdlWindow *win = new SdlWindow("win_test", 800, 480);
  win->create();
  SdlLayer *layer = new SdlLayer("button_layer");
  layer->set_zorder(2);
  layer->set_show(true);
  SdlLayer *video_layer = new SdlLayer("video_layer");
  video_layer->set_zorder(1);
  video_layer->set_show(true);
  SdlVideo *video_widget =
      new SdlVideo("video_widge", &win->array_, &win->window_,
                   win->renderer_mutex_, win->renderer_, 0, 0, 800, 480);
  video_layer->add_widget(static_cast<Widget *>(video_widget));
  SdlButton *pause_button = new SdlButton(
      "pause_button", "pause.png", &win->array_, &win->window_, win->renderer_);
  SdlButton *play_button = new SdlButton(
      "play_button", "resume.png", &win->array_, &win->window_, win->renderer_);
  SdlProgBar *prog_bar =
      new SdlProgBar("prog_bar", &win->window_, win->renderer_);
  struct play_pause_bar ppb;
  ppb.pause_button = pause_button;
  ppb.play_button = play_button;
  ppb.video = video_widget;
  prog_bar->set_action_callback(handle_bar_event, &ppb);
  layer->add_widget(static_cast<Widget *>(pause_button));
  layer->add_widget(static_cast<Widget *>(play_button));
  layer->add_widget(static_cast<Widget *>(prog_bar));
  win->add_layer(static_cast<Layer *>(layer));
  win->add_layer(static_cast<Layer *>(video_layer));
  video_widget->open("../../res/The_Shawshank_Redemption.mp4");
  video_widget->set_speed(1.0);
  win->show();
  delete video_widget;
  delete play_button;
  delete pause_button;
  delete prog_bar;
  delete win;
  return 0;
}
