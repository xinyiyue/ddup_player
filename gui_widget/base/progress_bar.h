#ifndef __PROGRESS_BAR__
#define __PROGRESS_BAR__

#include "gui_widget/base/widget.h"

typedef enum PLAYBACK_STATE {
    PLAYBACK_NON,
    PLAYBACK_PLAY,
    PLAYBACK_PAUSE,
    PLAYBACK_SEEK,
    PLAYBACK_EOS
} PLAYBACK_STATE_E; 

class ProgBar:public Widget {
    public:
        ProgBar(char *name):Widget(name) {};
        long long m_duration;
        long long m_currentTime;
        long long m_seekTime;
        PLAYBACK_STATE_E m_state;
        virtual void set_duration(long long duration) { m_duration = duration; };
        virtual void set_current_time(long long current) { m_currentTime = current; };
        virtual void set_playback_state(PLAYBACK_STATE state) { m_state = state; };
        long long get_seek_time(void) { return m_seekTime; };

        virtual bool is_dirty() { return true; };
        virtual int draw() { return true; };
        virtual int get_type() { return 0; };
        virtual int event_handler(void *event) { return 0; };
        virtual int set_event_resp_area(int x, int y, int w, int h) { return 0; };
        virtual void *get_window() { return nullptr; };
        virtual void *get_renderer() { return nullptr; };
};

#endif
