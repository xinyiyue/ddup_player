#ifndef __WIDGET__
#define __WIDGET__
enum WIDGET_TYPE {
    BUTTON_BASE_TYPE = 0,
    BAR_BASE_TYPE    = 10,
};

typedef void (*action_callback)(void *userdata, void* arg);

class Widget {
    public:
        Widget() { m_actionCB = nullptr; m_userdata = nullptr; }
        virtual int draw() = 0;
        virtual bool is_dirty() = 0;
        virtual int get_type() = 0;
        virtual void *get_window() = 0;
        virtual void *get_renderer() = 0;
        virtual int  event_handler(void *event) = 0;
        virtual int  set_event_resp_area(int x, int y, int w, int h) = 0;
        virtual void set_action_callback(action_callback cb, void *userdata) { m_actionCB = cb; m_userdata = userdata;};
    public:
        action_callback m_actionCB;
        void *m_userdata;
};

#endif
