#ifndef __SDL_TEXT_BOX_H__

#include <SDL2/SDL.h>

#include <vector>

#include "./gui_widget/sdl_impl/sdl_font.h"
#include "./gui_widget/sdl_impl/sdl_image.h"
#include "./gui_widget/sdl_impl/sdl_rect.h"
#include "gui_widget/sdl_impl/sdl_util.h"

class SdlTextBox {
 public:
  SdlTextBox(SDL_Renderer *renderer, SDL_Rect &rect, SdlFont *font);
  ~SdlTextBox();
  void set_info(int text_hight, SDL_Color *text, SDL_Color *background,
                SDL_Color *hightlight, SDL_Color *selected);
  void set_dirent_info(std::vector<DirentInfo *> *vec);
  void set_text(const char **text_array, int count);
  void update_first_line(int first_line);
  void get_line_info(int *display_line_cnt, int *text_line_cnt);
  int event_handler(void *event);
  int render_text_box();
  int get_action_line_name(char **name);
  const char *get_next_url();
  const char *get_prev_url();

 private:
  SdlRect *rect_;
  SdlRect *selected_rect_;
  SdlRect *hightlight_rect_;
  SDL_Rect action_rect_;
  int text_height_;
  SDL_Color text_color_;
  SdlFont *font_;
  const char **text_array_;
  int text_count_;
  SDL_Renderer *renderer_;
  int selected_line_;
  int last_selected_line_;
  int hightlight_line_;
  int first_line_;
  SdlImage *audio_;
  SdlImage *video_;
  SdlImage *picture_;
  SdlImage *dir_;
  SdlImage *unkown_;
  SdlImage *play_;
  SdlImage *enter_;
  char *entered_dir_name_;
  char *played_file_name_;
  std::vector<DirentInfo *> *vec_info_;
};

#endif