#ifndef __KISS_LAYER__
#define __KISS_LAYER__

#include <list>
#include <string>

#include "layer.h"

class SdlLayer:public Layer
{
    public:
        SdlLayer(char* name):Layer(name) {};
        virtual int event_handler(void *event) final {
            bool catched = false;
            for(auto widget:m_widgetList) {
                catched = widget->event_handler(event);
                if (catched)
                    break;
            }
            return catched;
        };
        
        virtual int update() final {
            for(auto widget:m_widgetList) {
                if (widget->is_dirty())
                    widget->draw();
            }
            return 0;
        };
};

#endif
