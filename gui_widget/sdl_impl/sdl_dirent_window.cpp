#include "./gui_widget/sdl_impl/sdl_dirent_window.h"

#include <dirent.h>
#include <sys/stat.h>

#include <string>

#include "log/ddup_log.h"

#define TAG "SdlDirentWindow"

SdlDirentWindow::SdlDirentWindow(SDL_Renderer *renderer, SDL_Rect &rect,
                                 SdlFont *font) {
  rect_ = rect;
  font_ = font;
  int fh = font_->get_font_height();
  up_rect_ = {rect_.x, rect_.y, rect_.w, fh};
  down_rect_ = {rect_.x, rect_.y + rect_.h - (fh + 20), rect_.w, (fh + 20)};
  close_rect_ = {rect_.x + rect_.w - fh, rect_.y, fh, fh};
  SDL_Rect title_rect = {rect_.x + 2, rect_.y + up_rect_.h, rect_.w - 6,
                         (fh + 20)};
  sdl_title_rect_ = new SdlRect(renderer, title_rect);
  renderer_ = renderer;
  is_text_updated_ = false;
  bg_image_ = new SdlImage(renderer, "win_rect.png");
  bg_upper_ = new SdlImage(renderer, "win_up.png");
  bg_down_ = new SdlImage(renderer, "win_down.png");
  bg_close_ = new SdlImage(renderer, "win_red.png");
  logo_ = new SdlImage(renderer, "ddup_player.png");
  SDL_Rect c_rect = {rect_.x + 2, rect_.y + up_rect_.h + title_rect.h,
                     rect_.w - 6,
                     rect_.h - title_rect.h - up_rect_.h - down_rect_.h};
  cbox_ = new SdlComboBox(renderer_, c_rect, font_);
  is_close_hl_ = false;
  is_close_ = false;
}

SdlDirentWindow::~SdlDirentWindow() {
  for (auto &info : dirent_info_list_) {
    free(info->file_name);
    free(info);
  }
  dirent_info_list_.clear();

  for (auto &it : dir_list_) {
    free(it->name);
    free(it);
  }
  dir_list_.clear();

  for (auto &it : label_list_) {
    delete it;
  }
  label_list_.clear();
  if (cbox_) {
    delete cbox_;
  }
}

int SdlDirentWindow::is_media_file(const char *filename) {
  const char *a_extensions[] = {".mp3", ".wav",  ".aac", ".flac",
                                ".pcm", ".opus", NULL};

  const char *v_extensions[] = {".mp4", ".avi", ".mov", ".mkv", NULL};

  const char *p_extensions[] = {".png", ".jpg", ".webp", ".gif", NULL};

  const char *dot = strrchr(filename, '.');
  if (dot == NULL) {
    return false;
  }
  size_t len = strlen(dot);
  for (int i = 0; a_extensions[i] != NULL; i++) {
    if (len == strlen(a_extensions[i]) &&
        strncasecmp(dot, a_extensions[i], len) == 0) {
      return 2;
    }
  }

  for (int i = 0; v_extensions[i] != NULL; i++) {
    if (len == strlen(v_extensions[i]) &&
        strncasecmp(dot, v_extensions[i], len) == 0) {
      return 1;
    }
  }

  for (int i = 0; p_extensions[i] != NULL; i++) {
    if (len == strlen(p_extensions[i]) &&
        strncasecmp(dot, p_extensions[i], len) == 0) {
      return 3;
    }
  }
  return -1;
}
int SdlDirentWindow::parse_dir(const char *path) {
  DIR *dir;
  struct dirent *entry;
  struct stat statbuf;

  for (auto &info : dirent_info_list_) {
    free(info->file_name);
    free(info);
  }
  dirent_info_list_.clear();
  for (auto &it : dir_list_) {
    free(it->name);
    free(it);
  }
  dir_list_.clear();
  for (auto &it : label_list_) {
    delete it;
  }
  label_list_.clear();
  if ((dir = opendir(path)) == nullptr) {
    LOGE(TAG, "Cannot open directory:%s", path);
    return 0;
  }
  cur_dir_ = path;
  char *temp = (char *)malloc(strlen(path) + 1);
  strcpy(temp, path);
  LOGI(TAG, "parse path:%s", temp);
  char *token;
  token = strtok(temp, "/");
  while (token != NULL) {
    DirInfo *dinfo = (DirInfo *)malloc(sizeof(DirInfo));
    dinfo->show = false;
    dinfo->name = (char *)malloc(strlen(token) + 1);
    strcpy(dinfo->name, token);
    dir_list_.push_back(dinfo);
    LOGI(TAG, "dir_list_ push:%s", dinfo->name);
    token = strtok(NULL, "/");
  }
  free(temp);

  while ((entry = readdir(dir)) != nullptr) {
    std::string fullPath = cur_dir_ + "/" + entry->d_name;

    if (stat(fullPath.c_str(), &statbuf) == -1 || !strcmp(entry->d_name, ".") ||
        !strcmp(entry->d_name, "..")) {
      LOGE(TAG, "Failed to get status of:%s", fullPath.c_str());
      continue;
    }
    int file_type = -1;
    if (S_ISDIR(statbuf.st_mode)) {
      file_type = 0;
    } else if (S_ISREG(statbuf.st_mode)) {
      file_type = is_media_file(entry->d_name);
    }
    if (file_type >= 0) {
      DirentInfo *info = (DirentInfo *)malloc(sizeof(DirentInfo));
      size_t len = strlen(entry->d_name) + 1;  // 加1为了'\0'
      info->file_name = static_cast<char *>(malloc(len));
      memcpy(info->file_name, entry->d_name, len);
      info->file_type = file_type;
      LOGI(TAG, "--> dir info:%s, type:%d", info->file_name, info->file_type);
      dirent_info_list_.push_back(info);
    }
  }
  closedir(dir);
  is_text_updated_ = true;
  return 0;
}

int SdlDirentWindow::check_dir_show_list(std::vector<DirInfo *> dir_list,
                                         const char *sstr, const char *fstr) {
  int width = rect_.w - 20 - logo_rect_.w;
  SDL_Rect ss = font_->get_text_rect(sstr);
  SDL_Rect fs = font_->get_text_rect(fstr);
  int target = 0;
  int tt = dir_list.size();
  for (int i = 0; i < tt; i++) {
    if (tt - i - 1 < 0) break;
    SDL_Rect r = font_->get_text_rect(dir_list[tt - i - 1]->name);
    if (target + r.w + fs.w <= width - ss.w - fs.w) {
      target += r.w + fs.w;
      dir_list[tt - i - 1]->show = true;
      LOGI(TAG, "---> get right index:%d, total:%d \n", tt - i - 1, target);

      SDL_Rect l = font_->get_text_rect(dir_list[i]->name);
      if (target + l.w + fs.w <= width - ss.w - fs.w) {
        target += l.w + fs.w;
        dir_list[i]->show = true;
        LOGI(TAG, "---> get left index:%d, total:%d \n", i, target);
      } else {
        break;
      }
    } else {
      SDL_Rect l = font_->get_text_rect(dir_list[i]->name);
      if (target + l.w + fs.w <= width - ss.w - fs.w) {
        target += l.w + fs.w;
        dir_list[i]->show = true;
        LOGI(TAG, "---> get left index:%d, total:%d \n", i, target);
      } else {
        break;
      }
    }
  }

  for (int i = 0; i < tt; i++) {
    LOGI(TAG, "--->dir title dir_list[%d]->name:%s, show:%d \n", i,
         dir_list[i]->name, dir_list[i]->show);
  }
  return 0;
}

int SdlDirentWindow::render_dirent() {
  SDL_Color white = {80, 80, 80, 255};
  SDL_Color gray = {235, 235, 235, 235};
  int last_x = 3;
  if (is_text_updated_) {
    sdl_title_rect_->set_attri(&rgba_white, 10, &gray);
    logo_rect_ = {
        sdl_title_rect_->get_rect().x + 10,
        rect_.y + up_rect_.h +
            (sdl_title_rect_->get_rect().h - font_->get_font_height()) / 2,
        font_->get_font_height(), font_->get_font_height()};
    check_dir_show_list(dir_list_, "...", " >");
    int len = dir_list_.size();
    bool has_set_fs = false;
    std::string tt;
    for (auto &info : dir_list_) {
      if (info->show) {
        tt = info->name;
        tt += " >";
      } else if (!info->show && !has_set_fs) {
        tt = "... >";
        has_set_fs = true;
      } else {
        continue;
      }
      LOGI(TAG, "new SdlLabel:%s, ", tt.c_str());
      SdlLabel *ll = new SdlLabel(
          renderer_, font_, tt.c_str(), rect_.x + 10 + logo_rect_.w + last_x,
          rect_.y + up_rect_.h +
              (sdl_title_rect_->get_rect().h - font_->get_font_height()) / 2);
      ll->set_attri(&white, &rgba_white, &rgba_white, 1);
      SDL_Rect *rect = ll->get_rect();
      last_x += rect->w;
      label_list_.push_back(ll);
    }
    cbox_->set_dirent_info(&dirent_info_list_);
    is_text_updated_ = false;
  }

  for (auto &it : label_list_) {
    it->render_label();
  }

  return 0;
}
int SdlDirentWindow::event_handler(void *event) {
  SDL_Event *e = (SDL_Event *)event;
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    if (point_in_rect(e->button.x, e->button.y, &close_rect_)) {
      is_close_ = true;
    }
  } else if (e->type == SDL_MOUSEMOTION) {
    if (point_in_rect(e->motion.x, e->motion.y, &close_rect_)) {
      is_close_hl_ = true;
    } else {
      is_close_hl_ = false;
    }
  }
  bool changed = false;
  for (int i = 0; i < label_list_.size(); i++) {
    label_list_[i]->event_handler(event);
    if (label_list_[i]->is_selected()) {
      cur_dir_ = "/";
      changed = true;
      int max_index = -1;
      if (label_list_[i]->get_text() == "... >") {
        for (int j = dir_list_.size() - 1; j > 0; j--) {
          if (dir_list_[j]->show) {
            continue;
          } else {
            max_index = j;
            break;
          }
        }
      } else {
        max_index = i;
      }
      for (int j = 0; j <= max_index; j++) {
        cur_dir_ += dir_list_[j]->name;
        cur_dir_ += "/";
        LOGI(TAG, "--->new dir:%s, ", cur_dir_.c_str());
      }
    }
  }
  if (changed) {
    parse_dir(cur_dir_.c_str());
  }
  cbox_->event_handler(event);
  return 0;
}

int SdlDirentWindow::render_dirent_window() {
  if (!is_close_) {
    bg_image_->render_image(nullptr, &rect_);
    bg_upper_->render_image(nullptr, &up_rect_);
    bg_down_->render_image(nullptr, &down_rect_);
    sdl_title_rect_->render_rect();
    sdl_title_rect_->render_edge();
    logo_->render_image(nullptr, &logo_rect_);
    if (is_close_hl_) {
      bg_close_->render_image(nullptr, &close_rect_);
    }
    render_dirent();
    cbox_->render_combo_box();
  }

  return 0;
}
