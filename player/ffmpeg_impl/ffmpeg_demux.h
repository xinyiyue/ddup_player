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
  FFmpegDemux(EventListener *listener) : Demux(listener){};
  virtual ~FFmpegDemux(){};
  virtual int prepare(const char *url) final;
  virtual int create_stream() final;
  virtual int stop() final;
  virtual int seek_impl(long long seek_time) final;
  virtual demux_event_t read_input_impl(av_data_s *data) final;
  virtual int free_data(void *data) final;

 private:
  AVFormatContext *fmt_ctx_;
  int audio_stream_index_;
  int video_stream_index_;
};

#endif
