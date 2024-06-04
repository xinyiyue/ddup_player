#include "kiss_sdl.h"
#include <iostream>

using namespace std;

enum PC_WIDGET_STATE {
    PC_WIDGET_UNKWON = 0,
    PC_WIDGET_PLAY,
    PC_WIDGET_PAUSE,
    PC_WIDGET_PAUSE_SEEK,
    PC_WIDGET_PLAY_SEEK,
    PC_WIDGET_STOP
};


//player progress bar
class ppBar {
    public:
        kiss_progressbar m_bar;
        kiss_label m_timeLabel;
        kiss_label m_durLabel;
        SDL_Renderer *m_pRenderer;
        kiss_window* m_pWindow;
        int m_duration;
        ppBar(kiss_window* win, SDL_Renderer *renderer, int duration); 
        int draw(int current_time, int state);
};

ppBar::ppBar(kiss_window* win, SDL_Renderer *renderer, int duration) {
    m_pWindow = win;
    m_pRenderer = renderer;
    kiss_progressbar_new(&m_bar, win, win->rect.x, win->rect.y + kiss_screen_height - 20, kiss_screen_width);
    kiss_label_new(&m_timeLabel,win, (char*)"0:0:0", win->rect.x, win->rect.y + kiss_screen_height - 18);
    kiss_label_new(&m_durLabel, win, (char*)"0:0:0", kiss_screen_width - 50, kiss_screen_height - 18);
    m_duration = duration;
    int hour = m_duration / 3600;
    int min  = (m_duration % 3600) / 60;
    int sec  = (m_duration % 3600) % 60;
    char buf[10];
    int len = snprintf(buf,sizeof(buf),"%d:%02d:%02d",hour, min, sec);
    if (len >= (int)sizeof(buf)) {
        cout << "time buffer is overflow:" << buf << endl;
    }
    kiss_string_copy(m_durLabel.text, KISS_MAX_LABEL, buf, NULL);
    m_bar.step = (1/(float)m_duration);
    m_bar.run = 1;
}

int ppBar::draw(int current_time, int state) {
    if (current_time > m_duration) {
        cout << "current time is bigger than duration" << endl;
        return -1;
    }
    char buf[10];
    int draw;
    int hour = current_time / 3600;
    int min  = (current_time % 3600) / 60;
    int sec  = (current_time % 3600) % 60;
    int len = snprintf(buf,sizeof(buf),"%d:%02d:%02d",hour, min, sec);
    if (len >= (int)sizeof(buf)) {
        cout << "time buffer is overflow:" << buf << endl;
    }
    kiss_string_copy(m_timeLabel.text, KISS_MAX_LABEL, buf, NULL);
    if (state == PC_WIDGET_PAUSE_SEEK || state == PC_WIDGET_PLAY_SEEK) {
       m_bar.fraction= current_time /(float)m_duration;
    } else if (state == PC_WIDGET_PLAY) {
       kiss_progressbar_event(&m_bar, NULL, &draw);
    }
    kiss_progressbar_draw(&m_bar, m_pRenderer);
    kiss_label_draw(&m_timeLabel, m_pRenderer);
    kiss_label_draw(&m_durLabel, m_pRenderer);
    return 0;
}

class pcButton {
    public:
        kiss_image m_image;
        SDL_Renderer *m_pRenderer;
        kiss_window *m_pWindow;
        SDL_Rect m_rect;
        int m_posX;
        int m_posY;
        pcButton(kiss_window* win, SDL_Renderer *renderer,kiss_array *objects, char* fname);
        ~pcButton();
        int draw();
};

pcButton::pcButton(kiss_window* win, SDL_Renderer *renderer, kiss_array *objects, char* fname) {
    m_pRenderer = renderer;
    m_pWindow = win;
    kiss_image_new(&m_image, fname, objects, m_pRenderer);
    m_posX = m_pWindow->rect.w / 2 - m_image.w / 2;
    m_posY = m_pWindow->rect.h / 2 - m_image.h / 2;
    kiss_makerect(&m_rect, 0, 0, m_image.w, m_image.h);
}

pcButton::~pcButton() {
   //kiss_clean(&m_objects); 
}

int pcButton::draw() {
    kiss_renderimage(m_pRenderer, m_image, m_posX, m_posY, &m_rect);
    return 0;
}

//player control widget
class pcWidget {
    public:
        SDL_Renderer *m_pRenderer;
        kiss_window m_window;
        kiss_array m_objects;
        kiss_image m_image1;
        kiss_image m_image2;
        kiss_image m_image3;
        pcButton *m_pause_button;
        pcButton *m_resume_button;
        ppBar *m_bar;
        PC_WIDGET_STATE m_state;
        int m_seek_time;
        int m_curr_time;
        ~pcWidget();
        int create_window(char* win_name, int win_x, int win_y);
        int create_progress_bar(int duration);
        int create_control_button();
        int draw();
        int init_media_source();
        int update_media_source();
        int update_state(SDL_Event *e);
};

pcWidget::~pcWidget() {
    if (m_bar) {
        delete m_bar;
        m_bar = nullptr;
    }
    if (m_pause_button) {
        delete m_pause_button;
        m_pause_button = nullptr;
    }
    if (m_resume_button) {
        delete m_resume_button;
        m_resume_button = nullptr;
    }
}

int pcWidget::create_window(char* win_name, int win_x, int win_y) {
    m_pRenderer = kiss_init(win_name, &m_objects, win_x, win_y);
    if (!m_pRenderer)
        return 1;
    m_state = PC_WIDGET_PLAY;
    cout << "set state to:" << m_state << ", when create window" << endl;
    m_seek_time = -1;
    m_curr_time = 0;
    kiss_window_new(&m_window, NULL, 0, 0, 0, win_x, win_y);
    return 1;
}

int pcWidget::create_progress_bar(int duration) {
    cout << "create progress bar, state:" << m_state << endl;
    m_bar = new ppBar(&m_window, m_pRenderer, duration);
    return 0;
}

int pcWidget::create_control_button() {
    cout << "create button, state:" << m_state << endl;
    m_pause_button = new pcButton(&m_window, m_pRenderer, &m_objects, (char*)"pause.png");
    m_resume_button = new pcButton(&m_window, m_pRenderer, &m_objects, (char*)"resume.png");
    return 0;
}


int pcWidget::init_media_source() {
    kiss_image_new_scaled(&m_image1, (char*)"picture1.png", &m_objects, kiss_screen_width, kiss_screen_height, m_pRenderer);
    kiss_image_new_scaled(&m_image2, (char*)"picture2.png", &m_objects, kiss_screen_width, kiss_screen_height, m_pRenderer);
    kiss_image_new_scaled(&m_image3, (char*)"picture3.png", &m_objects, kiss_screen_width, kiss_screen_height, m_pRenderer);
    //kiss_image_new(&m_image1, "picture1.png", &m_objects, m_pRenderer);
    //kiss_image_new(&m_image2, "picture2.png", &m_objects, m_pRenderer);
    //kiss_image_new(&m_image3, "picture3.png", &m_objects, m_pRenderer);
    //kiss_image_new(&m_image_pause, "pause.png", &m_objects, m_pRenderer);
    //kiss_image_new(&m_image_resume, "resume.png", &m_objects, m_pRenderer);
    //kiss_image_new_scaled(&m_image_pause, "pause.png", &m_objects,100, 100, m_pRenderer);
    //kiss_image_new_scaled(&m_image_resume, "resume.png", &m_objects, 100, 100, m_pRenderer);
    return 0;
}

int pcWidget::update_state(SDL_Event *e) {
    if (!m_window.visible || !e) return 0;
    if (!m_window.focus && (!m_window.wdw ||
        (m_window.wdw && !m_window.wdw->focus)))
        return 0;
    if (e->type == SDL_MOUSEBUTTONDOWN && kiss_pointinrect(e->button.x, e->button.y,&(m_bar->m_bar.rect))) {
        m_seek_time = (e->button.x / (float)m_window.rect.w) * m_bar->m_duration;
        if (m_state == PC_WIDGET_PLAY) {
            m_state = PC_WIDGET_PLAY_SEEK;
        } else if (m_state == PC_WIDGET_PAUSE) {
            m_state = PC_WIDGET_PAUSE_SEEK;
        }
        cout << "check seek state" << endl;
        return 1;
    }
   

    // check if mouse down point is outside of progress bar, then play or pause
    if (e->type == SDL_MOUSEBUTTONDOWN && !kiss_pointinrect(e->button.x, e->button.y,&(m_bar->m_bar.rect))) {
        if (m_state == PC_WIDGET_PLAY) {
            m_state = PC_WIDGET_PAUSE;
        } else if (m_state == PC_WIDGET_PAUSE) {
            m_state = PC_WIDGET_PLAY;
        }
        cout << "check non seek state" << endl;
        return 1;
    }
    return 0;
}

int pcWidget::update_media_source() {
        SDL_Rect rect;
        kiss_makerect(&rect, 0, 0, kiss_screen_width, kiss_screen_height);
        int rr = m_curr_time % 3;
        if (rr == 0) {
            kiss_renderimage(m_pRenderer, m_image1,0, 0, &rect);
        } else if (rr == 1) {
            kiss_renderimage(m_pRenderer, m_image2,0, 0, &rect);
        } else {
            kiss_renderimage(m_pRenderer, m_image3,0, 0, &rect);
        }
        
        return 0;
}

int pcWidget::draw() {
    SDL_Event e;
    bool quit = false;
    int count = 0;
    m_window.visible = 1;
    while (!quit) {
        SDL_Delay(500);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
            int ret = update_state(&e);
            if (ret > 0) {
                if (m_state == PC_WIDGET_PLAY || m_state == PC_WIDGET_PAUSE)
                    count = 1;
                cout << "update to new state:" << m_state << endl;
            }
        }
        SDL_RenderClear(m_pRenderer);
        kiss_window_draw(&m_window, m_pRenderer);

        //1. update media source
        update_media_source();       
        //2. update resume pause button
        if (m_state == PC_WIDGET_PLAY && count > 0) {
            m_resume_button->draw();
            count--;
        } else if (m_state == PC_WIDGET_PAUSE && count > 0) {
            m_pause_button->draw();
        }
        if (m_state == PC_WIDGET_PLAY) {
            m_curr_time++;
        } else if (m_state == PC_WIDGET_PLAY_SEEK || m_state == PC_WIDGET_PAUSE_SEEK) {
            m_curr_time = m_seek_time;
        }
        //3. update progress bar
        m_bar->draw(m_curr_time, m_state);
        if (m_state == PC_WIDGET_PAUSE_SEEK) {
            m_state = PC_WIDGET_PAUSE;
        } else if (m_state == PC_WIDGET_PLAY_SEEK) {
            m_state = PC_WIDGET_PLAY;
        }

        SDL_RenderPresent(m_pRenderer);
    }
    kiss_clean(&m_objects);
    return 1;
}

int main(int argc, char **argv)
{
    pcWidget *pw = new pcWidget();
    pw->create_window((char*)"ppWidget", 800, 400);
    pw->create_progress_bar(300);
    pw->create_control_button();
    pw->init_media_source();
    pw->draw();

    return 0;
}
