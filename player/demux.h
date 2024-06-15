#ifndef __DEMUX_H_H__
#define __DEMUX_H_H__

#include <pthread.h>

#include <string>

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
  Demux(EventListener *listener, char *url);
  virtual ~Demux();

  virtual void notify_event(int event_type, void *ret);
  virtual void notify_error(int error_type);

  virtual int init();
  virtual int open() = 0;
  virtual int create_stream() = 0;
  virtual int play(float speed) = 0;
  virtual int pause() = 0;
  virtual int seek(long long seek_time) = 0;
  virtual int stop() = 0;
  virtual int close() = 0;
  virtual demux_event_t read_input_data(av_data_s *data) = 0;

  void request_input_data();
  static void *input_thread(void *arg);

 public:
  Stream *audio_stream_;
  Stream *video_stream_;
  std::string url_;
  pthread_t input_thread_id_;
  pthread_mutex_t mutex_;
  pthread_cond_t cond_;
  bool input_thread_exit_;
  EventListener *listener_;
  bool ready_to_read_data_;
};

#endif
