#include "kiss_sdl.h"
#include <iostream>

using namespace std;

class ppbar {
    public:
        kiss_progressbar m_bar;
        kiss_label m_timeLabel;
        kiss_label m_durLabel;
        SDL_Renderer *m_pRenderer;
        kiss_window* m_pWindow;
        int m_duration;
        ppbar(kiss_window* win, SDL_Renderer *renderer, int duration); 
        int draw(int current_time, bool seek, bool pause);
        int check_seek_poistion(SDL_Event *event);
        int check_pause_resume(SDL_Event *event);
};

ppbar::ppbar(kiss_window* win, SDL_Renderer *renderer, int duration) {
    m_pWindow = win;
    m_pRenderer = renderer;
    kiss_progressbar_new(&m_bar, win, win->rect.x, win->rect.y + kiss_screen_height - 20, kiss_screen_width);
    kiss_label_new(&m_timeLabel,win, "0:0:0", win->rect.x, win->rect.y + kiss_screen_height - 18);
    kiss_label_new(&m_durLabel, win, "0:0:0", kiss_screen_width - 50, kiss_screen_height - 18);
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

int ppbar::draw(int current_time, bool seek, bool pause) {
    if (current_time > m_duration) {
        cout << "current time is bigger than duration" << endl;
        return -1;
    }
    char buf[10];
    int draw;
    int hour = current_time / 3600;
    int min  = (current_time % 3600) / 60;
    int sec  = (current_time % 3600) % 60;
    snprintf(buf,10,"%d:%d:%d",hour, min, sec);
    kiss_string_copy(m_timeLabel.text, KISS_MAX_LABEL, buf, NULL);
    cout << "draw bar at state:" <<  pause << endl;
    if (seek) {
       m_bar.fraction= current_time /(float)m_duration;
    } else if (pause) {
       kiss_progressbar_event(&m_bar, NULL, &draw);
    }
    kiss_progressbar_draw(&m_bar, m_pRenderer);
    kiss_label_draw(&m_timeLabel, m_pRenderer);
    kiss_label_draw(&m_durLabel, m_pRenderer);
    return 0;
}

int ppbar::check_seek_poistion(SDL_Event *event)
{
    if (!m_pWindow || !m_pWindow->visible || !event) return 0;
    if (!m_pWindow->focus && (!m_pWindow->wdw ||
        (m_pWindow->wdw && !m_pWindow->wdw->focus)))
        return -1;
    if (event->type == SDL_MOUSEBUTTONDOWN && kiss_pointinrect(event->button.x, event->button.y,&(m_bar.rect))) {
        int seek_time = (event->button.x / (float)kiss_screen_width) * m_duration;
        return seek_time;
    }
    return -1;
}

int ppbar::check_pause_resume(SDL_Event *event)
{
    if (!m_pWindow || !m_pWindow->visible || !event) return 0;
    if (!m_pWindow->focus && (!m_pWindow->wdw ||
        (m_pWindow->wdw && !m_pWindow->wdw->focus)))
        return -1;
    if (event->type == SDL_MOUSEBUTTONDOWN && !kiss_pointinrect(event->button.x, event->button.y,&(m_bar.rect))) {
        return 1;
    }
    return -1;
}


class ppWidget {
    public:
        SDL_Renderer *m_pRenderer;
        kiss_window m_window;
        kiss_array m_objects;
        kiss_image m_image1;
        kiss_image m_image2;
        kiss_image m_image3;
        kiss_image m_image_pause;
        kiss_image m_image_resume;
        ppbar *m_bar;
        int create_window(char* win_name, int win_x, int win_y);
        int add_bar(int duration);
        int draw_window();
        int add_image();
};

int ppWidget::create_window(char* win_name, int win_x, int win_y) {
    m_pRenderer = kiss_init("ppb", &m_objects, win_x, win_y);
    if (!m_pRenderer)
        return 1;
    kiss_window_new(&m_window, NULL, 0, 0, 0, win_x, win_y);
    return 1;
}

int ppWidget::add_bar(int duration) {
    m_bar = new ppbar(&m_window, m_pRenderer, duration);
    return 0;
}

int ppWidget::add_image() {
    kiss_image_new_scaled(&m_image1, "picture1.png", &m_objects, kiss_screen_width, kiss_screen_height, m_pRenderer);
    kiss_image_new_scaled(&m_image2, "picture2.png", &m_objects, kiss_screen_width, kiss_screen_height, m_pRenderer);
    kiss_image_new_scaled(&m_image3, "picture3.png", &m_objects, kiss_screen_width, kiss_screen_height, m_pRenderer);
    //kiss_image_new(&m_image1, "picture1.png", &m_objects, m_pRenderer);
    //kiss_image_new(&m_image2, "picture2.png", &m_objects, m_pRenderer);
    //kiss_image_new(&m_image3, "picture3.png", &m_objects, m_pRenderer);
    kiss_image_new(&m_image_pause, "pause.png", &m_objects, m_pRenderer);
    kiss_image_new(&m_image_resume, "resume.png", &m_objects, m_pRenderer);
    //kiss_image_new_scaled(&m_image_pause, "pause.png", &m_objects,100, 100, m_pRenderer);
    //kiss_image_new_scaled(&m_image_resume, "resume.png", &m_objects, 100, 100, m_pRenderer);
    return 0;
}


int ppWidget::draw_window() {
    SDL_Event e;
    bool quit = false;
    m_window.visible = 1;
    bool seek = false;
    int time = 0;
    bool play_pause = true;//ture: play, false: pause
    int count = 0;
    while (!quit) {
        SDL_Delay(500);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
            int ret = m_bar->check_seek_poistion(&e);
            if (ret > 0) {
                time = ret - 1;
                seek = true;
            } else {
               ret = m_bar->check_pause_resume(&e);
               if (ret > 0) {
                   if (play_pause) {
                       play_pause = false;
                   } else {
                       play_pause = true;
                   }
                   count = 1;
               }
            }
        }
        SDL_RenderClear(m_pRenderer);
        kiss_window_draw(&m_window, m_pRenderer);
        SDL_Rect rect;
        kiss_makerect(&rect, 0, 0, kiss_screen_width, kiss_screen_height);
        SDL_Rect rect2;
        kiss_makerect(&rect2, 0, 0, 180, 180);
        int rr = time % 3;
        if (rr == 0) {
            kiss_renderimage(m_pRenderer, m_image1,0, 0, &rect);
        } else if (rr == 1) {
            kiss_renderimage(m_pRenderer, m_image2,0, 0, &rect);
        } else {
            kiss_renderimage(m_pRenderer, m_image3,0, 0, &rect);
        }
        if (play_pause == true && count > 0) {
            kiss_renderimage(m_pRenderer, m_image_resume, kiss_screen_width/2 - 90, kiss_screen_height/2 - 90, &rect2);
            if (count > 0) {
                count--;
            }
        } else if (play_pause == false && count > 0) {
            kiss_renderimage(m_pRenderer, m_image_pause, kiss_screen_width/2 - 90, kiss_screen_height/2 - 90, &rect2);
        }
        if (play_pause) {
            time++;
        }
        m_bar->draw(time, seek, play_pause);
        SDL_RenderPresent(m_pRenderer);
        seek = false;
    }
    kiss_clean(&m_objects);
    return 1;
}

int main(int argc, char **argv)
{
    ppWidget *pw = new ppWidget();
    pw->create_window("ppWidget", 800, 400);
    pw->add_bar(300);
    pw->add_image();
    pw->draw_window();

    return 0;
}
