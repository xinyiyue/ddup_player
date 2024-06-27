#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "player/demux.h"
#include "player/event_listener.h"
#include "player/pipeline.h"
#include "player/stream.h"

class AutoLock {
 public:
  AutoLock(pthread_mutex_t *mutex) {
    mutex_ = mutex;
    pthread_mutex_lock(mutex_);
  };
  ~AutoLock() { pthread_mutex_unlock(mutex_); }

 private:
  pthread_mutex_t *mutex_;
};

int CreatePipeline(EventListener *listener, Pipeline **pipeline);
int CreateDemux(EventListener *listener, Demux **demux);
int CreateStream(stream_type_t type, void *stream_info, Stream **Stream);
int CreateDecoder(void *codec_param, Stream *stream, Decoder **decoder);

#endif
