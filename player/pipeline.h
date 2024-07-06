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
  virtual int prepare(char *url);
  virtual int play(float speed);
  virtual int pause();
  virtual int seek(long long seek_time);
  virtual int stop();
  virtual int close();

  virtual void notify_event(int event_type) final;
  virtual void notify_error(int error_type) final;

 private:
  Demux *demux_;
  Decoder *decoder_;
  EventListener *listener_;
};

#endif
