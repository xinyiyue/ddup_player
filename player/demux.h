#ifndef __DEMUX_H_H__
#define __DEMUX_H_H__

#include <pthread.h>

#include <string>

#include "player/decoder.h"
#include "player/event_listener.h"
#include "player/stream.h"

typedef enum DEMUX_EVENT_TYPE {
  DEMUX_OK,
  DEMUX_EOS,
  DEMUX_EAGAN,
  DEMUX_ERROR,
} demux_event_t;

class Demux : public EventListener {
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
  virtual int free_input_data(void *data) = 0;

  void request_input_data();

 public:
  Stream *audio_stream_;
  Stream *video_stream_;
  Decoder *video_decoder_ = nullptr;
  Decoder *audio_decoder_ = nullptr;
  std::string url_;

  pthread_mutex_t mutex_;
  pthread_cond_t cond_;
  pthread_mutex_t cmd_mutex_;
  bool input_thread_exit_;
  EventListener *listener_;
  bool ready_to_read_data_;

 private:
  std::thread input_thread_;

 private:
  int init();
  int uninit();
  virtual int create_stream() = 0;
  virtual int create_decoder() = 0;
  void *input_thread(void *arg);
};

#endif
