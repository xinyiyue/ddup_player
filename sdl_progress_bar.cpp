#include <iostream>
#include "sdl_progress_bar.h"

using namespace std;

SdlProgBar::SdlProgBar(char *name,
                       kiss_window* win,
                       SDL_Renderer *renderer):ProgBar(name) {
    m_pWindow = win;
    m_pRenderer = renderer;
    m_dirty = true;
    m_state = PLAYBACK_PLAY;
    kiss_progressbar_new(&m_bar, win, win->rect.x, win->rect.y + kiss_screen_height - 20, kiss_screen_width);
    m_rect.x = m_bar.rect.x;
    m_rect.y = m_bar.rect.y;
    m_rect.w = m_bar.rect.w;
    m_rect.h = m_bar.rect.h;
    kiss_label_new(&m_timeLabel,win, (char*)"0:0:0", win->rect.x, win->rect.y + win->rect.h - 18);
    kiss_label_new(&m_durLabel, win, (char*)"0:0:0", win->rect.w - 50, win->rect.h - 18);
}
        
bool SdlProgBar::is_dirty() {
    return m_dirty;
}

void SdlProgBar::set_duration(long long duration) {
    m_duration = duration;
    int hour = m_duration / 3600;
    int min  = (m_duration % 3600) / 60;
    int sec  = (m_duration % 3600) % 60;
    char buf[10];
    snprintf(buf,10,"%d:%d:%d",hour, min, sec);
    kiss_string_copy(m_durLabel.text, KISS_MAX_LABEL, buf, NULL);
    m_bar.step = (1/(float)m_duration);
    cout << "bar step:" << m_bar.step << endl;
    m_bar.run = 1;
}

void SdlProgBar::set_current_time(long long current) {
    char buf[10];
    if (current <= 0) {
        return;
    }
    m_currentTime = current;
    int hour = m_currentTime/ 3600;
    int min  = (m_currentTime % 3600) / 60;
    int sec  = (m_currentTime % 3600) % 60;
    snprintf(buf,10,"%d:%d:%d",hour, min, sec);
    kiss_string_copy(m_timeLabel.text, KISS_MAX_LABEL, buf, NULL);
    m_bar.fraction = m_currentTime /(float)m_duration + 0.01;
}

int SdlProgBar::draw() {
    kiss_progressbar_draw(&m_bar, m_pRenderer);
    kiss_label_draw(&m_timeLabel, m_pRenderer);
    kiss_label_draw(&m_durLabel, m_pRenderer);
    return 0;
}

int SdlProgBar::get_type() {
    return 0;
}

int SdlProgBar::event_handler(void *event) {
    SDL_Event *e = (SDL_Event *)event;
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        // check if mouse down point is outside of progress bar, then play or pause
        if (!kiss_pointinrect(e->button.x, e->button.y,&m_rect)) {
            if (m_state == PLAYBACK_PLAY) {
                m_state = PLAYBACK_PAUSE;
            } else if (m_state == PLAYBACK_PAUSE) {
                m_state = PLAYBACK_PLAY;
            }
        } else {
            m_seekTime = (e->button.x / (float)m_pWindow->rect.w) * m_duration;
            set_current_time(m_seekTime);
            m_state = PLAYBACK_SEEK;
            cout << "check seek state" << endl;
        }
        m_dirty = true;
        if (m_actionCB) {
            action a;
            a.state = m_state;
            a.seek_time = m_seekTime;
            m_actionCB(m_userdata, &a);
        }
        return 1;
    }
    return 0;
}

int SdlProgBar::set_event_resp_area(int x, int y, int w, int h) {
    return 0;
}

void *SdlProgBar::get_window() {
    return m_pWindow;
}

void *SdlProgBar::get_renderer() {
    return m_pRenderer;
}

