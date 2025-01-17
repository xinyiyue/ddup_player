#ifndef __DDUP__PLAYER_H__
#define __DDUP__PLAYER_H__

#include "event_listener.h"
#include "pipeline.h"

typedef enum DDupState {
  DDUP_STATE_NULL,
  DDUP_STATE_OPEN,
  DDUP_STATE_PLAY,
  DDUP_STATE_PAUSE,
  DDUP_STATE_SEEK,
  DDUP_STATE_STOP,
  DDUP_STATE_CLOSE
} ddup_state_t;

class DDupPlayer : public EventListener {
 public:
  DDupPlayer(EventListener *listener);
  ~DDupPlayer();
  int open();
  int prepare(const char *url);
  int set_speed(float speed);
  int seek(long long seek_time);
  int stop();
  int close();
  ddup_state_t get_state() { return state_; };
  long long get_current_time();
  virtual void notify_event(int event_type, void *ret) final;
  virtual void notify_error(int error_type) final;

 private:
  Pipeline *pipeline_;
  ddup_state_t state_;
  long long current_time_;
  long long duration_;
};

#endif
