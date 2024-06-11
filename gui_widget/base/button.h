#ifndef __BUTTON__
#include <string>
#include "gui_widget/base/widget.h"

class Button:public Widget {
    public:
        int m_posX;
        int m_posY;
        int m_width;
        int m_height;
        std::string m_skinFile;
        Button(int x, int y, int w, int h, char *skin):Widget() {
            m_posX = x;
            m_posY = y;
            m_width = w;
            m_height = h;
            m_skinFile = skin; 
        };
        
        Button(int x, int y, int w, int h, char *skin, char *name):Widget(name) {
            m_posX = x;
            m_posY = y;
            m_width = w;
            m_height = h;
            m_skinFile = skin; 
        };

        virtual ~Button() {};
        virtual int draw() { return 0; };
        virtual bool is_dirty() { return true; }
        virtual int get_type() { return BUTTON_BASE_TYPE; };
        virtual void *get_window() { return nullptr; };
        virtual void *get_renderer() { return nullptr; };
        virtual int  event_handler(void *event) { return 0; };
        virtual int  set_event_resp_area(int x, int y, int w, int h) { return 0; };
};
#endif
