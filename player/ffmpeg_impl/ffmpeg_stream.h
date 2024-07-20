#ifndef __FFMPEG_STREAM__H__
#define __FFMPEG_STREAM__H__

#include "player/stream.h"
extern "C" {
#include "libavformat/avformat.h"
}
class FFmpegStream : public Stream {
 public:
  FFmpegStream(stream_type_t type, AVStream *stream, EventListener *listener)
      : Stream(type, listener) {
    ff_stream_ = stream;
  };
  virtual ~FFmpegStream(){};
  virtual int create_decoder() final;
  virtual int create_processer() final;

 private:
  AVStream *ff_stream_;
};

#endif
