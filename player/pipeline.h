#ifndef __DDUP__PIPELINE_H__
#define __DDUP__PIPELINE_H__

#include "player/demux.h"
#include "player/event_listener.h"

class Pipeline : public EventListener {
 public:
  Pipeline(EventListener *listener);
  ~Pipeline();
  int open();
  int prepare(char *url);
  int play(float speed);
  int pause();
  int seek(long long seek_time);
  int stop();
  int close();

  virtual void notify_event(int event_type) final;
  virtual void notify_error(int error_type) final;

 private:
  Demux *demux_;
  EventListener *listener_;
};

#endif
