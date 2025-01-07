#ifndef __SDL_DIRENT_WINDOW_H__
#define __SDL_DIRENT_WINDOW_H__
#include <SDL2/SDL.h>

#include <string>
#include <vector>

#include "./gui_widget/sdl_impl/sdl_combo_box.h"
#include "./gui_widget/sdl_impl/sdl_label.h"
#include "base_util/media_type.h"
#include "gui_widget/sdl_impl/sdl_util.h"

typedef struct DirInfo {
  char *name;
  bool show;
} DirInfo;

class SdlDirentWindow {
 public:
  SdlDirentWindow(SDL_Renderer *renderer, SDL_Rect &rect, SdlFont *font);
  ~SdlDirentWindow();
  int parse_dir(const char *dir);
  int event_handler(void *event);
  int render_dirent_window();
  std::string get_play_url() { return play_url_; }
  const char *get_next_url();
  const char *get_prev_url();

 private:
  int render_dirent();
  int check_dir_show_list(std::vector<DirInfo *> dir_list, const char *sstr,
                          const char *fstr);
  SdlFont *font_;
  SdlComboBox *cbox_;
  SDL_Renderer *renderer_;
  SDL_Rect rect_;
  SDL_Rect up_rect_;
  SDL_Rect down_rect_;
  SDL_Rect close_rect_;
  SDL_Rect logo_rect_;
  SdlRect *sdl_title_rect_;
  std::vector<DirentInfo *> dirent_info_list_;
  std::vector<DirInfo *> dir_list_;
  std::vector<SdlLabel *> label_list_;
  SdlLabel *hide_label_;
  int hide_label_idx_;
  std::string cur_dir_;
  bool is_text_updated_;
  SdlImage *bg_image_;
  SdlImage *bg_upper_;
  SdlImage *bg_down_;
  SdlImage *bg_close_hl_;
  SdlImage *bg_close_;
  SdlImage *logo_;
  bool is_close_;
  bool is_close_hl_;
  std::string play_url_;
};

#endif
