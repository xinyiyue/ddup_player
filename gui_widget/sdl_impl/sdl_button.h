#ifndef __KISS_BUTTON__
#define __KISS_BUTTON__

#include "gui_widget/base/button.h"
#include "third_party/kiss_sdl/kiss_sdl.h"

class SdlButton: public Button {
    public:
        SdlButton( char *name,
                   char *skin, 
                   kiss_array *arr,
                   kiss_window* win,
                   SDL_Renderer *renderer,
                   int x, int y, int w, int h);
        SdlButton( char *name,
                   char *skin,
                   kiss_array *arr,
                   kiss_window* win,
                   SDL_Renderer *renderer);
        virtual ~SdlButton() {};
        virtual int draw() final;
        virtual bool is_dirty() final;
        virtual int get_type() final;
        virtual void *get_window() final;
        virtual void *get_renderer() final;
        virtual void set_show(bool show, int time_ms) final;
        virtual int  event_handler(void *event) final;
        virtual int  set_event_resp_area(int x, int y, int w, int h) final;
        SDL_Renderer *m_pRenderer;
        kiss_window *m_pWindow;
        SDL_TimerID m_hideDelayTimerId;
    private:
        kiss_image m_image;
        SDL_Rect m_rect;
        SDL_Rect m_resp_rect;
};
#endif
