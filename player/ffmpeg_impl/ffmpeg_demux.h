#ifndef __FFMPEG_DEMUX_H_H__
#define __FFMPEG_DEMUX_H_H__

#include <pthread.h>

#include <string>

#include "player/demux.h"
#include "player/stream.h"

extern "C" {
#include "third_party/ffmpeg-7.0.1/include/libavformat/avformat.h"
}

class FFmpegDemux : public Demux {
 public:
  FFmpegDemux(EventListener *listener, char *url) : Demux(listener, url){};
  virtual ~FFmpegDemux(){};
  virtual int open() final;
  virtual int create_stream() final;
  virtual int play(float speed) final;
  virtual int pause() final;
  virtual int seek(long long seek_time) final;
  virtual int stop() final;
  virtual int close() final;
  virtual demux_event_t read_input_data(av_data_s *data) final;

 private:
  AVFormatContext *fmt_ctx_;
  int audio_stream_index_;
  int video_stream_index_;
};

#endif
