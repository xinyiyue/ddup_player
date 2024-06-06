#ifndef __SDL_PROGRESS_BAR__
#define __SDL_PROGRESS_BAR__

#include "kiss_sdl.h"
#include "widget.h"
#include "progress_bar.h"
#include "sdl_button.h"

typedef struct action {
    PLAYBACK_STATE_E state;
    long long seek_time;
} action;

class SdlProgBar:public ProgBar {
    public:
        SDL_Renderer *m_pRenderer;
        kiss_window *m_pWindow;
        kiss_progressbar m_bar;
        kiss_label m_timeLabel;
        kiss_label m_durLabel;
    public:
        SdlProgBar(char *name,
                   kiss_window* win,
                   SDL_Renderer *renderer);
        virtual ~SdlProgBar() {};

        virtual bool is_dirty() final;
        virtual int draw() final;
        virtual int get_type() final;
        virtual int event_handler(void *event) final;
        virtual int set_event_resp_area(int x, int y, int w, int h) final;
        virtual void *get_window() final;
        virtual void *get_renderer() final;
        
        virtual void set_duration(long long duration) final;
        virtual void set_current_time(long long current) final;
    private:
        SDL_Rect m_rect;
};

#endif
