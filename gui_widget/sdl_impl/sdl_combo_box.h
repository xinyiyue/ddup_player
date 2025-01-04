#ifndef __SDL_COMBO_BOX_H__

#include <SDL2/SDL.h>

#include <vector>

#include "./gui_widget/sdl_impl/sdl_text_box.h"
#include "./gui_widget/sdl_impl/sdl_vscroll_bar.h"
#include "gui_widget/sdl_impl/sdl_util.h"

class SdlComboBox {
 public:
  SdlComboBox(SDL_Renderer *renderer, SDL_Rect &rect, SdlFont *font);
  SdlComboBox(SDL_Renderer *renderer, SdlFont *font, SdlTextBox *text_box,
              SdlVScrollBar *vscroll_bar);
  ~SdlComboBox();
  void set_dirent_info(std::vector<DirentInfo *> *vec);
  void set_text(const char **text_array, int count);
  int event_handler(void *event);
  int render_combo_box();
  int get_action_line_name(char **name);

 private:
  SdlFont *font_;
  SdlTextBox *text_box_;
  SdlVScrollBar *vscroll_bar_;
  SDL_Renderer *renderer_;
  bool need_srcollbar_;
};

#endif