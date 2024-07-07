#ifndef __SDL_VIDEO_SINK__H__
#define __SDL_VIDEO_SINK__H__

#include "player/sink.h"

class SDLVideoSink : public Sink {
 public:
  SDLVideoSink()
      : Sink(VIDEO_SINK){

        };
  virtual ~SDLVideoSink() = default;

  virtual int open(void* codec_param) final;
  virtual int start() final;
  virtual int stop() final;
  virtual int close() final;
};

#endif
