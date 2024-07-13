#include <ostream>
#include <iostream>
#include "gui_widget/sdl_impl/sdl_window.h"

using namespace std;

SdlWindow::SdlWindow(char *name, int w, int h):Window(name, w, h) {
    m_exit = false;
}

SdlWindow::~SdlWindow() {

}

int SdlWindow::create() {
    m_pRenderer = kiss_init((char *)m_name.c_str(), &m_array, m_width, m_height);
    if (!m_pRenderer)
        return 1;
    kiss_window_new(&m_window, NULL, 0, 0, 0, m_width, m_height);
    m_window.visible = 1;
    return 0;
}

int SdlWindow::event_handler(void *event) {
    bool catched = false;
    for(auto layer:m_layerList) {
        if (!layer->m_show) {
            continue;
        }
        catched = layer->event_handler(event);
        if (catched) {
            break;
        }
    }
    return catched;
}

int SdlWindow::show() {
    cout << "window:" << m_name << " show !!" << endl;
    update();
    SDL_Event event;
    while(!m_exit) {
        while(SDL_WaitEvent(&event)) {
            if (event.type == SDL_QUIT) {
                cout << "show get quit event" << endl;
                m_exit = true;
                break;
            }
            if (event_handler(&event)) {
                update();
            }
        }

    }
    
    kiss_clean(&m_array);
    cout << "window:" << m_name << " eixt!!!" << endl;
    return 0;
}

int SdlWindow::update() {
    SDL_RenderClear(m_pRenderer);
    kiss_window_draw(&m_window, m_pRenderer);
    for(auto layer:m_layerList) {
        if (!layer->m_show) {
            continue;
        }
        layer->update();
    }
    SDL_RenderPresent(m_pRenderer);
    return 0;
}



