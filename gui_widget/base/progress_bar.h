#ifndef __PROGRESS_BAR__
#define __PROGRESS_BAR__

#include "gui_widget/base/widget.h"

typedef enum PLAYBACK_STATE {
  PLAYBACK_NON,
  PLAYBACK_PLAY,
  PLAYBACK_PAUSE,
  PLAYBACK_SEEK,
  PLAYBACK_STOP,
  PLAYBACK_EOS
} PLAYBACK_STATE_E;

class ProgBar : public Widget {
 public:
  ProgBar(const char *name) : Widget(name){};
  virtual void set_duration(long long duration) { duration_ = duration; };
  virtual void set_current_time(long long current) { current_time_ = current; };
  virtual void set_playback_state(PLAYBACK_STATE state) { state_ = state; };
  long long get_seek_time(void) { return seek_time_; };

  virtual bool is_dirty() { return true; };
  virtual int draw() { return true; };
  virtual int get_type() { return 0; };
  virtual int event_handler(void *event) { return 0; };
  virtual int set_event_resp_area(int x, int y, int w, int h) { return 0; };
  virtual void *get_window() { return nullptr; };
  virtual void *get_renderer() { return nullptr; };

  long long duration_;
  long long current_time_;
  long long seek_time_;
  PLAYBACK_STATE_E state_;
};

#endif
