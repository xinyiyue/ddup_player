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
  DEMUX_STATE_SEEK,
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
  virtual int prepare(const char *url);
  virtual int set_speed(float speed);
  virtual int seek(long long seek_time);
  virtual int seek_impl(long long seek_time) = 0;
  virtual int stop();
  virtual int close();
  virtual demux_event_t read_input_impl(av_data_s *data) = 0;
  virtual int free_data(void *data) override { return 0; };

  void set_state(demux_state_t state);
  demux_state_t get_state() { return state_; };
  int read_data_abort();
  const char *get_state_string(demux_state_t state);
  int read_input(av_data_s *data);
  void check_wait_ready();
  void set_ready();

 public:
  Stream *audio_stream_;
  Stream *video_stream_;
  std::string url_;
  pthread_mutex_t mutex_;
  pthread_cond_t cond_;
  pthread_mutex_t cmd_mutex_;
  bool input_thread_exit_;

 protected:
  long long duration_;
  long long position_;

 private:
  virtual int create_stream() = 0;
  void input_thread(void);
  std::thread input_thread_;
  Fifo *audio_fifo_;
  Fifo *video_fifo_;
  demux_state_t state_;
  bool audio_eos_;
  bool video_eos_;
};

#endif
