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

typedef enum DDupErrorType {
  DDUP_NETWORK_ERROR,
  DDUP_DECODE_ERROR,
  DDUP_PLAYBACK_ERROR,
} ddup_error_t;

typedef enum DDupEventType {
  DDUP_EVENT_OPENED,
  DDUP_EVENT_PLAYED,
  DDUP_EVENT_PAUSED,
  DDUP_EVENT_SEEKED,
  DDUP_EVENT_STOPPED,
  DDUP_EVENT_CLOSED,
  DDUP_EVENT_CURRENT_TIME,
} ddup_event_t;

typedef void (*error_listener_func)(ddup_error_t err);

class DDupPlayer : public EventListener {
 public:
  DDupPlayer(error_listener_func listener);
  ~DDupPlayer();
  int open(char *url);
  int play(float speed);
  int pause();
  int seek(long long seek_time);
  int stop();
  int close();
  long long get_current_time();

  virtual void notify_event(int event_type, void *ret) final;
  virtual void notify_error(int error_type) final;

 private:
  Pipeline *pipeline_;
  ddup_state_t state_;
  long long current_time_;
  error_listener_func error_listener_;
};

#endif
