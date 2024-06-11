#ifndef __WINDOW__
#define __WINDOW__


#include <algorithm>
#include <string>
#include <list>
#include <string.h>
#include "layer.h"

class Window{
    public:
        Window(char *name, int w, int h) { m_name = name; m_width = w; m_height = h; };
        virtual int create() = 0;
        virtual int update() = 0;
        virtual int show() = 0;

        virtual int add_layer(Layer* layer) {
            if (m_layerList.size() == 0) {
                m_layerList.insert(m_layerList.begin(), layer);
            } else {
                std::list<Layer *>::iterator it = m_layerList.begin();
                while (it != m_layerList.end()) {
                    if ((*it)->m_zorder > layer->m_zorder) {
                        m_layerList.insert(it, layer);
                        break;
                    }
                    it++;
                }
            }
            return 0;
        };

        virtual int remove_layer(Layer *layer) {
            auto it = std::find(m_layerList.begin(), m_layerList.end(), layer);
            if (it != m_layerList.end()) {
                m_layerList.erase(it);
            }
            return 0;
        };

        virtual Layer *get_layer_by_name(char *name) {
            Layer* layer = nullptr;
            for (auto it : m_layerList) {
                if (!strcmp((char *)it->m_name.c_str(), name)) {
                    layer = it;
                    break;
                }
            }
            return layer;
        };
        int get_window_info(int *w, int *h) { *w = m_width; *h = m_height; return 0;};
    public:
        int m_width;
        int m_height;
        std::string m_name;
        std::list<Layer*> m_layerList;
};
#endif
