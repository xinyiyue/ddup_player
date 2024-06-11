#ifndef __LAYER__
#define __LAYER__

#include <list>
#include <string>

#include "gui_widget/base/widget.h"

class Layer
{
    public:
        Layer(char* name) { m_name = name; }
        int add_widget(Widget *widget) {
            std::list<Widget *>::iterator it = m_widgetList.end();
            m_widgetList.insert(it, widget);
            return 0;
        };
        int remove_widget();
        int set_show(bool show) {
            m_show = show;
            return 0;
        };
        int set_zorder(int zorder) { m_zorder = zorder; return 0; }
        int get_zorder() { return m_zorder; }
        virtual int event_handler(void *event) = 0;
        virtual int update() = 0;
    public:
        std::string m_name;
        int m_zorder;
        bool m_show;
        std::list<Widget*> m_widgetList;
};

#endif
