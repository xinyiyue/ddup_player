#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <iostream>
#include "sdl_window.h"
#include "sdl_button.h"
#include "sdl_layer.h"

using namespace std;

void handle_button_event(void *userdata, void* event) {
    SdlButton *button = (SdlButton *)userdata;
    SDL_Event *sdl_event = (SDL_Event *)event;
    cout << "kiss button recieve:" << sdl_event->type << endl;
}

int main(int argc, char* argv[]) {
    SdlWindow *win = new SdlWindow((char *)"win_test", 800, 480);
    win->create();
    SdlLayer *layer = new SdlLayer((char *)"button");
    layer->set_zorder(1);
    layer->set_show(true);
    SdlButton *button = new SdlButton((char *)"pause.png", &win->m_array, &win->m_window, win->m_pRenderer);
    button->set_action_callback(handle_button_event, NULL);
    //button->set_event_resp_area(0, 0, 800, 400);
    layer->add_widget(static_cast<Widget *>(button));
    win->add_layer(static_cast<Layer *>(layer));
    win->show();
    delete win;
    return 0;
}
