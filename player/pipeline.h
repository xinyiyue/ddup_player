#ifndef __DDUP__PIPELINE_H__
#define __DDUP__PIPELINE_H__

#include "player/decoder.h"
#include "player/demux.h"
#include "player/event_listener.h"

class Pipeline : public EventListener {
 public:
  Pipeline(EventListener *listener);
  ~Pipeline();
  virtual int open();
  virtual int prepare(const char *url);
  virtual int set_speed(float speed);
  virtual int seek(long long seek_time);
  virtual int stop();
  virtual int close();

  virtual void notify_event(int event_type) final;
  virtual void notify_error(int error_type) final;

 private:
  Demux *demux_;
};

#endif
