#ifndef __DEMUX_H_H__
#define __DEMUX_H_H__

#include <pthread.h>

#include <string>

#include "player/event_listener.h"
#include "player/fifo_controller.h"
#include "player/stream.h"

typedef enum DEMUX_EVENT_TYPE {
  DEMUX_OK,
  DEMUX_EOS,
  DEMUX_EAGAN,
  DEMUX_ERROR,
} demux_event_t;

typedef enum DEMUX_STATE_TYPE {
  DEMUX_STATE_OPEN,
  DEMUX_STATE_PREPARE,
  DEMUX_STATE_PLAY,
  DEMUX_STATE_PAUSE,
  DEMUX_STATE_PLAY_SEEK,
  DEMUX_STATE_PAUSE_SEEK,
  DEMUX_STATE_STOP,
  DEMUX_STATE_CLOSE,
  DEMUX_STATE_EOS
} demux_state_t;

class Demux : public EventListener, public BufferProducer, public FreeHandler {
 public:
  Demux(EventListener *listener);
  virtual ~Demux();

  virtual void notify_event(int event_type, void *ret);
  virtual void notify_error(int error_type);

  virtual int open();
  virtual int prepare(char *url);
  virtual int play(float speed);
  virtual int pause();
  virtual int seek(long long seek_time);
  virtual int stop();
  virtual int close();
  virtual demux_event_t read_input_data(av_data_s *data) = 0;
  virtual int free_data(void *data) override { return 0; };

  void set_ready();
  void set_state(demux_state_t state);
  void check_wait_ready();
  demux_state_t get_state() { return state_; };
  int check_discard_data();
  int read_data_abort();

 public:
  Stream *audio_stream_;
  Stream *video_stream_;
  std::string url_;
  pthread_mutex_t mutex_;
  pthread_cond_t cond_;
  pthread_mutex_t cmd_mutex_;
  bool input_thread_exit_;
  EventListener *listener_;

 private:
  virtual int create_stream() = 0;
  void *input_thread(void *arg);
  std::thread input_thread_;
  Fifo *audio_fifo_;
  Fifo *video_fifo_;
  demux_state_t state_;
};

#endif
