#ifndef __SDL_VIDEO_H_H__
#define __SDL_VIDEO_H_H__

#include "third_party/kiss_sdl/kiss_sdl.h"
#include "third_party/FIFO/FIFO.h"
#include "gui_widget/base/video.h"

typedef struct renderBuffer {
    char *data;
    int len;
    int width;
    int height;
    int pixel;
    int frameRate;
} renderBuffer_s;

class SdlVideo:public Video {
    public:
        SdlVideo(char *name,
                 kiss_array *arr, 
                 kiss_window* win,
                 SDL_Renderer *renderer,
                 int x, int y, int w, int h);
        virtual ~SdlVideo() {
            SDL_DestroyCond(m_cond);
            SDL_DestroyMutex(m_mutex);
        };

        virtual int open(char * url) final;
        virtual int start() final;
        virtual int stop() final;
        virtual int seek(long long seekTime) final;
        virtual int pause_resume(bool pause) final;
        virtual int close() final;

        virtual int draw() final;
        virtual int get_type() final;
        virtual int event_handler(void *event) final;
        virtual int set_event_resp_area(int x, int y, int w, int h) final;
        virtual void *get_window() final;
        virtual void *get_renderer() final;


        static int render_thread(void *data);
    public:
        SDL_Renderer *m_pRenderer;
        kiss_window *m_pWindow;
        SDL_Rect m_dstRect;
        SDL_Rect m_srcRect;
        SDL_Texture* m_texture;
        int width;
        int height;
        SDL_Thread *m_renderThread;
        fifo_t m_fifo;
        SDL_cond *m_cond; 
        SDL_mutex *m_mutex;
        bool m_exit;
};

#endif
