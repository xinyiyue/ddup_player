#include <iostream>
#include "sdl_button.h"

#define KISS_BUTTON_TYPE BUTTON_BASE_TYPE + 1
using namespace std;
SdlButton::SdlButton(char *skin,
                     kiss_array *arr, 
                     kiss_window* win,
                     SDL_Renderer *renderer):Button(0, 0, 0, 0, skin) {
    m_pRenderer = renderer;
    m_pWindow = win;
    kiss_image_new(&m_image, skin, arr, renderer);
    m_posX = m_pWindow->rect.w / 2 - m_image.w / 2;
    m_posY = m_pWindow->rect.h / 2 - m_image.h / 2;
    m_width = m_image.w;
    m_height = m_image.h;
    kiss_makerect(&m_rect, 0, 0, m_image.w, m_image.h);
    kiss_makerect(&m_resp_rect, m_posX, m_posY, m_image.w, m_image.h);
}

SdlButton::SdlButton(char *skin,
                     kiss_array *arr, 
                     kiss_window* win,
                     SDL_Renderer *renderer,
                     int x, int y, int w, int h):Button(x, y, w, h, skin) {
    m_pRenderer = renderer;
    m_pWindow = win;
    kiss_image_new_scaled(&m_image, skin, arr, w, h, m_pRenderer);
    m_posX = x;
    m_posY = y;
    kiss_makerect(&m_rect, 0, 0, w, h);
    kiss_makerect(&m_resp_rect, m_posX, m_posY, m_image.w, m_image.h);
}

int SdlButton::set_event_resp_area(int x, int y, int w, int h) {
    m_resp_rect.x = x;
    m_resp_rect.y = y;
    m_resp_rect.w = w;
    m_resp_rect.h = h;
    return 0;
}

int SdlButton::draw() {
    int ret = 0;
    cout << "draw button" << endl;
    ret = kiss_renderimage(m_pRenderer, m_image, m_posX, m_posY, &m_rect);
    if (!ret)
        m_dirty = false;
    return ret;
}

int  SdlButton::event_handler(void *event) {
    if (m_actionCB) {
        SDL_Event *e = (SDL_Event *)event;
        if (e->type == SDL_MOUSEBUTTONDOWN) {
            cout << "got SDL_MOUSEBUTTONDOWN" << endl;
        }
        if (e->type == SDL_MOUSEBUTTONDOWN && kiss_pointinrect(e->button.x, e->button.y, &m_resp_rect)) {
            m_actionCB(m_userdata, event);
            cout << "kiss_button handle inner click" << endl;
            m_dirty = true;
            return 1;
        }
    }
    return 0;
}

bool SdlButton::is_dirty() {
    return m_dirty;
}

int SdlButton::get_type() {
    return KISS_BUTTON_TYPE;
}

void *SdlButton::get_window() {
    return (void *)m_pWindow;
}

void *SdlButton::get_renderer() {
    return (void *)m_pRenderer;
}
