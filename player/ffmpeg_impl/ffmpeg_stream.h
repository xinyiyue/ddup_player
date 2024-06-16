#ifndef __FFMPEG_STREAM__H__
#define __FFMPEG_STREAM__H__

#include "player/stream.h"
extern "C" {
#include "third_party/ffmpeg-7.0.1/include/libavformat/avformat.h"
}
class FFmpegStream : public Stream {
 public:
  FFmpegStream(stream_type_t type, AVStream *stream) : Stream(type) {
    ff_stream_ = stream;
  };
  virtual ~FFmpegStream(){};
  virtual int stream_on() final;
  virtual int play(float speed) final;
  virtual int pause() final;
  virtual int stream_off() final;

 private:
  AVStream *ff_stream_;
};

#endif
