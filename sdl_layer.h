#ifndef __KISS_LAYER__
#define __KISS_LAYER__

#include <list>
#include <string>
#include <iostream>
#include "layer.h"

class SdlLayer:public Layer
{
    public:
        SdlLayer(char* name):Layer(name) {};
        virtual int event_handler(void *event) final {
            bool catched = false;
            SDL_Event *e = (SDL_Event *)event;
            for(auto widget:m_widgetList) {
                catched = widget->event_handler(event);
                if (catched) {
                    std::cout << "layer:" << m_name << ",widget:" << widget->m_name << ", catched event:" << e->type << std::endl;
                    break;
                }
            }
            return catched;
        };
        
        virtual int update() final {
            for(auto widget:m_widgetList) {
                std::cout << "widget:" << widget->m_name << ", is_dirty:" << widget->is_dirty() << std::endl;
                if (widget->is_dirty())
                    widget->draw();
            }
            return 0;
        };
};

#endif
