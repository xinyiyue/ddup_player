#ifndef __KISS_WINDOW__
#define __KISS_WINDOW__

#include <pthread.h>
#include "gui_widget/base/window.h"
#include "third_party/kiss_sdl/kiss_sdl.h"

class SdlWindow:public Window{
    public:
        SdlWindow(char *name, int w, int h);
        ~SdlWindow();
        virtual int create() final;
        virtual int update() final;
        virtual int show() final;
        virtual int event_handler(void *event) final;
        SDL_Renderer *m_pRenderer;
        kiss_window m_window;
        kiss_array m_array;
    private:
        bool m_exit;
};
#endif
