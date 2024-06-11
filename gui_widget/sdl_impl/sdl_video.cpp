#include <iostream>
#include "gui_widget/sdl_impl/sdl_user_event.h"
#include "gui_widget/sdl_impl/sdl_video.h"

using namespace std;

SdlVideo::SdlVideo(char *name,
                   kiss_array *arr, 
                   kiss_window* win,
                   SDL_Renderer *renderer,
                   int x, int y, int w, int h):Video(name) {
    m_pRenderer = renderer;
    m_pWindow = win;
    m_dirty = false;
    m_exit = false;
    kiss_makerect(&m_dstRect, x, y, w, h);
    m_fifo = fifo_create(3, sizeof(renderBuffer_s *));
    if (!m_fifo) {
        cout << "create fifo failed" << endl;
    }
    m_cond = SDL_CreateCond();
    m_mutex = SDL_CreateMutex();
}

int SdlVideo::render_thread(void *data) {
    SdlVideo *video = (SdlVideo *)data;
    int sleepTime = 1/(float)30 * 1000; //ms
    while(!video->m_exit) {
        if(fifo_is_empty(video->m_fifo)) {
            renderBuffer *buff;
            fifo_get(video->m_fifo, &buff);
            if (!buff) {
                cout << "get buffer null from fifo" << endl;
                continue;
            }

            if (!video->m_texture) {
                video->m_texture = SDL_CreateTexture(video->m_pRenderer, buff->pixel, SDL_TEXTUREACCESS_STREAMING, buff->width, buff->height);
                video->width = buff->width;
                video->height = buff->height;
                kiss_makerect(&video->m_srcRect, 0, 0, buff->width, buff->height);
                sleepTime = 1/(float)buff->frameRate;
            }
            //SDL_UpdateYUVTexture();
            SDL_UpdateTexture(video->m_texture,  NULL, buff, buff->width);
            
            SDL_LockMutex(video->m_mutex);
            video->m_dirty = true;
            SDL_UnlockMutex(video->m_mutex);
            SDL_Event event;
            event.type = SDL_USER_EVENT_VIDEO_UPDATE;
            event.user.data1 = NULL;
            event.user.data2 = NULL;
            SDL_PushEvent(&event);
        }
        SDL_Delay(sleepTime);
    }
    return 0;
}

int SdlVideo::open(char * url) {
    m_renderThread = SDL_CreateThread(SdlVideo::render_thread, "render_Thread", this);
    return 0;
}

int SdlVideo::start() {

    return 0;
}

int SdlVideo::stop() {

    return 0;
}

int SdlVideo::seek(long long seekTime) {

    return 0;
}

int SdlVideo::pause_resume(bool pause) {

    return 0;
}

int SdlVideo::close() {
    m_exit = true;
    return 0;
}

int SdlVideo::draw() {
	SDL_RenderCopy(m_pRenderer, m_texture, &m_srcRect, &m_dstRect);
    SDL_LockMutex(m_mutex);
    m_dirty = false;
    SDL_UnlockMutex(m_mutex);
    return 0;
}

int SdlVideo::get_type() {

    return 0;
}

int SdlVideo::event_handler(void *event) {
    SDL_Event *e = (SDL_Event *)event;
    if (e->type == SDL_USER_EVENT_VIDEO_UPDATE) {
        cout << "catch event video update event, will update video picture" << endl;
        return 1; 
    }
    return 0;
}

int SdlVideo::set_event_resp_area(int x, int y, int w, int h) {

    return 0;
}

void *SdlVideo::get_window() {

    return m_pWindow;
}

void *SdlVideo::get_renderer() {

    return m_pRenderer;
}

