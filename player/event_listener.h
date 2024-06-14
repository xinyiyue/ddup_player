#ifndef __EVENT_LISTENER__H__
#define __EVENT_LISTENER__H__

class EventListener {
  virtual void notify_event(int event_type, void *ret){};
  virtual void notify_error(int error_type){};
};

#endif
