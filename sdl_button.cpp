#include <iostream>
#include "sdl_user_event.h"
#include "sdl_button.h"

#define KISS_BUTTON_TYPE BUTTON_BASE_TYPE + 1
using namespace std;

unsigned int delay_hide_timer_handler(unsigned int interval, void* param) {
    SdlButton *button = (SdlButton *)param;
    SDL_RemoveTimer(button->m_hideDelayTimerId);
    button->m_hideDelayTimerId = 0;
    cout << "delay handler, button name:" << button->m_name <<", call  set show:" << false << ", delay:" << 0 << endl;
    button->set_show(false, 0);
    SDL_Event event;
    event.type = SDL_USER_EVENT_REFRESH;
    event.user.data1 = NULL;
    event.user.data2 = NULL;
    SDL_PushEvent(&event);
    return 0;
}

SdlButton::SdlButton(char *name,
                     char *skin,
                     kiss_array *arr, 
                     kiss_window* win,
                     SDL_Renderer *renderer):Button(0, 0, 0, 0, skin, name) {
    m_pRenderer = renderer;
    m_pWindow = win;
    m_dirty = false;
    m_hideDelayTimerId = -1;
    kiss_image_new(&m_image, skin, arr, renderer);
    m_posX = m_pWindow->rect.w / 2 - m_image.w / 2;
    m_posY = m_pWindow->rect.h / 2 - m_image.h / 2;
    m_width = m_image.w;
    m_height = m_image.h;
    kiss_makerect(&m_rect, 0, 0, m_image.w, m_image.h);
    kiss_makerect(&m_resp_rect, m_posX, m_posY, m_image.w, m_image.h);
}

SdlButton::SdlButton(char *name,
                     char *skin,
                     kiss_array *arr, 
                     kiss_window* win,
                     SDL_Renderer *renderer,
                     int x, int y, int w, int h):Button(x, y, w, h, skin, name) {
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
    ret = kiss_renderimage(m_pRenderer, m_image, m_posX, m_posY, &m_rect);
    if (!ret) {
        if (m_hideDelayTime > 0) {
            cout << "sdlbutton name:" << m_name << ", start timer, delay:" << m_hideDelayTime  << endl;
            m_hideDelayTimerId  = SDL_AddTimer(m_hideDelayTime, delay_hide_timer_handler, this);
            m_hideDelayTime = 0;
        }
    }
    return ret;
}

void SdlButton::set_show(bool show, int time_ms) {
    cout << "SdlButton name:" << m_name << " set show:" << show << ", delay:" << time_ms << endl;
    Widget::set_show(show, time_ms);
    SDL_Event event;
    event.type = SDL_USER_EVENT_REFRESH;
    event.user.data1 = NULL;
    event.user.data2 = NULL;
    SDL_PushEvent(&event);
}

int  SdlButton::event_handler(void *event) {
    SDL_Event *e = (SDL_Event *)event;
    if (m_actionCB) {
        if (m_dirty && e->type == SDL_MOUSEBUTTONDOWN && kiss_pointinrect(e->button.x, e->button.y, &m_resp_rect)) {
            m_actionCB(m_userdata, event);
            cout << "kiss_button handle inner click" << endl;
            return 1;
        }
    }
    if (e->type == SDL_USER_EVENT_REFRESH) {
           cout << "catch event 100, force update button" << endl;
           return 1; 
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
