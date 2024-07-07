#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

#include <iostream>

#include "gui_widget/sdl_impl/sdl_button.h"
#include "gui_widget/sdl_impl/sdl_layer.h"
#include "gui_widget/sdl_impl/sdl_progress_bar.h"
#include "gui_widget/sdl_impl/sdl_window.h"
#include "player/ddup_player.h"
#include "third_party/FIFO/FIFO.h"

using namespace std;

struct play_pause_bar {
  SdlButton *pause_button;
  SdlButton *play_button;
};

void handle_bar_event(void *userdata, void *event) {
  action *a = (action *)event;
  struct play_pause_bar *ppb = (struct play_pause_bar *)userdata;
  cout << "bar_event handler,state::" << a->state
       << ", seek_time:" << a->seek_time << endl;
  if (a->state == PLAYBACK_PAUSE) {
    ppb->pause_button->set_show(true, 0);
    ppb->play_button->set_show(false, 0);
  } else if (a->state == PLAYBACK_PLAY) {
    ppb->pause_button->set_show(false, 0);
    ppb->play_button->set_show(true, 3000);
  }
}

int test_fifo() {
  char data[] = "Hola Mundo1234";
  fifo_t myfifo;

  myfifo = fifo_create(10, sizeof(char));

  if (myfifo == NULL) {
    cout << "Cannot create FIFO... halting!" << endl;
  } else {
    cout << "FIFO created successfully" << endl;
  }

  cout << "\r\nFILLING FIFO WITH DATA..." << endl;
  for (unsigned int i = 0; i < sizeof(data); i++) {
    cout << "Add item to FIFO: " << data[i];
    if (fifo_add(myfifo, &data[i])) {
      cout << " OK!" << endl;
    } else {
      cout << " FAIL !!!" << endl;
    }
  }

  cout << "\r\nGETTING DATA FROM FIFO..." << endl;
  // dump data from FIFO to serial monitor
  while (!fifo_is_empty(myfifo)) {
    char c;
    fifo_get(myfifo, &c);
    cout << "get fifo:" << c << endl;
  }
  cout << "destory fifo" << endl;
  fifo_destory(myfifo);
  return 0;
}

void error_listener(ddup_error_t err) {}

int test_player() {
  DDupPlayer *player = new DDupPlayer(error_listener);
  player->open();
  player->prepare((char *)"../res/The_Shawshank_Redemption.mp4");
  player->play(1.0);
  sleep(5);
  player->seek(5000);
  sleep(5);
  player->stop();
  player->close();
  delete player;
  return 0;
}

int main(int argc, char *argv[]) {
  test_player();
  // test_fifo();
#if 0
    SdlWindow *win = new SdlWindow((char *)"win_test", 800, 480);
    win->create();
    SdlLayer *layer = new SdlLayer((char *)"button");
    layer->set_zorder(1);
    layer->set_show(true);
    SdlButton *pause_button = new SdlButton((char *)"pause_button", (char *)"pause.png", &win->m_array, &win->m_window, win->m_pRenderer);
    SdlButton *play_button = new SdlButton((char *)"play_button", (char *)"resume.png", &win->m_array, &win->m_window, win->m_pRenderer);
    SdlProgBar *prog_bar = new SdlProgBar((char *)"prog_bar", &win->m_window, win->m_pRenderer);
    struct play_pause_bar ppb;
    ppb.pause_button = pause_button;
    ppb.play_button = play_button;
    prog_bar->set_action_callback(handle_bar_event, &ppb);
    prog_bar->set_duration(1000);
    layer->add_widget(static_cast<Widget *>(pause_button));
    layer->add_widget(static_cast<Widget *>(play_button));
    layer->add_widget(static_cast<Widget *>(prog_bar));
    win->add_layer(static_cast<Layer *>(layer));
    win->show();
    delete play_button;
    delete pause_button;
    delete prog_bar;
    delete win;
#endif
  return 0;
}
